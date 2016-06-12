//
// aeb5005
// September 8th, 2015
// 
// Note: To compile the below program that uses clock_gettime, you 
// need to link with librt.a (the real-time library) 
// by specifying -lrt on your compile line.

// DESCRIPTION OF PROGRAM
// This program, application.c, is designed to first mark the time using 
// clock_gettime(CLOCK_MONOTONIC, &foo). Next, if the program is passed arguments,
// the program continues. ./timer is designed to pass ./application two arguments. 
// The first arg (arg1, in timer.c ptr1) represents the time it takes to fork(), which is the start time.
// The second arg (arg2, in timer.c ptr2) is designed to either be a 1 or a 2. Depending on the value, 
// the filename is altered. This is so when this program is called by multiple fork() calls, 
// both applications aren't doing file I/O to the same file, which causes many problems.
// Arguments are passed as strings, so strtoll() is used to convert the start time to uint64_t.
// strcmp() is used to compare the string representation of 1 or 2 in arg2 to "1" or "2".

// Next, numerous file I/O actitives take place, including, creating a file, writing/reading to file,
// comparing what has been written to what has been read back, and file deletion. This takes place 
// in a loop to increase runtime, as the program is designed to be run for longer than a few seconds.
// The loop size, and various other aspects, are controlled by a macro at the top called "SIZE" 
// The basic structure of my file I/O is as follows: 
// Two int arrays are created, both size SIZE. A loop fills the first array with SIZE number of random
// integers between 0-99. 
// A file pointer is created. fopen is used to point to a new binary file. If NULL is returned, an error 
// message is printed. 
// fwrite() is used to write the the first array to the file. The function fwrite() writes nmemb elements 
// of data, each size bytes long, to the stream pointed to by stream, obtaining them from the location 
// given by ptr. On success, fwrite() returns the number of items written, and this number equals the number
// of bytes transferred only when size is 1. If an error occurs, or EOF is reached, the return value is a
// very short count or zero. 
// The file pointer is then set back to the beginning of the file using fseek(). Upon error, fseek() returns -1.
// fread() is then used to write the file to the second empty array. fread() is very similar to fwrite().
// Error checking is handled in a similar way. I also check if the end of file was reached prematurely.
// The file pointer is done being used at this point, so it is closed using fclose().
// I used memcmp() to compare the two arrays, in order to compare what was written to what was read back.
// If the return value is 0, they are equal.

// You will notice a number of commented out print statements. These are used to indicate success/failure
// of the various I/O operations, and are were simply commented out. 

#include <stdio.h>
#include <stdlib.h> //for exit() def
#include <string.h> // for memcmp
#include <stdint.h> //for uint64 def
#include <sys/time.h> //for gettimeofday
#include <time.h>

#define SIZE 20000 //used for size of array r, r2, program loop. adjust to control program run time
#define BILLION 1000000000L

void srand();
size_t fwrite(const void *p, size_t size, size_t nmemb, FILE *stream);
size_t fread(void *p, size_t size, size_t nmemb, FILE *stream);
void delay(int milliseconds);
long long int strtoll(const char *nptr, char **endptr, int base);

int main(int argc, char **argv)
{
	struct timespec end_child;
	if(clock_gettime(CLOCK_MONOTONIC, &end_child) == -1) { //capture time
		printf("error getting time from start of application. exiting.\n");
		exit(1);
	} 

	if (argc > 1) { //if passed arguments
		uint64_t start, endChildTime, diff;
		start = strtoll(argv[1], NULL, 10); //converts string initialTime to long long 
		endChildTime = BILLION * end_child.tv_sec + end_child.tv_nsec; //store end time
		diff = endChildTime - start;

		int i,c; //counters
		int ret; //return value
		//int removeFlag = 0; //used for counting
		char filename[10]; //stores name of file to be executed

		if(strcmp(argv[2],"1") == 0) { //identifies if child1 or child2 called the application
			printf("Time to start up ./application inside of Child 1 = %llu nanoseconds\n", (long long unsigned int) diff);
			char filename[] = "test1.bin";
		} else if(strcmp(argv[2],"2") == 0) {
			char filename[] = "test2.bin";
			printf("Time to start up ./application inside of Child 2 = %llu nanoseconds\n", (long long unsigned int) diff);
		} else {
			printf("error passing correct arg2 value.\n");
			exit(1);
		}
		
		for (c=0;c<SIZE;c++) {  //loop to extend runtime
			srand( time(NULL) ); //initialize random seed
			
			int r[SIZE]; //creating initial array of random ints that will write to test.bin
			int r2[SIZE]; //creating array that will be read to from test.bin

			//populate the initial array with random numbers between 0-99
			for(i=0; i<SIZE; i++) {
				r[i] = rand()%100;
				//printf("%d\n", r[i]);
			}

			//create a file
			//this opens the file for writing, creating the file if necessary
			FILE *fp; 
			if((fp = fopen(filename, "wb+")) == NULL) {
				printf("error opening file.\n");
				exit(1);
			}
			
			//writes content to file
			if(fwrite(r, sizeof(int), SIZE, fp) != SIZE) {
				printf("file write error.\n");
			}

			//can also use rewind(fp);
			if( fseek(fp, 0, SEEK_SET) == -1) {
				printf("fseek error.\n");
			}

			//read content from file
			if(fread(r2, sizeof(int), SIZE, fp) != SIZE) {
				if(feof(fp)) 
					printf("premature end of file.\n");
				else
					printf("file read error.\n");
			}

			//closing file pointer
			fclose(fp);

			//Compare what has been written to what has been read back
			if (memcmp(r, r2, SIZE) != 0) {
				printf("not equal memcmp.\n");
			} else {
				//printf("equal memcmp\n");
			}

			//delete file
			ret = remove(filename);	
			if(ret == 0) {
				//printf("File deleted successfully\n");
				//removeFlag++;
			} else {
				printf("Error: unable to delete the file.\n");
			}
		}
	}
	return 0;
}
