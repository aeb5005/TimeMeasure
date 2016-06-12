//
// aeb5005
// September 8th, 2015
// 
// Note: To compile the below program that uses clock_gettime, you 
// need to link with librt.a (the real-time library) 
// by specifying -lrt on your compile line.

// ABSTRACT OF APPROACH AND TECHNIQUES
// DESCRIPTION OF PROGRAM
// This program, timer.c, is designed to execute the application (./application) present
// in the same directory as timer.c. Additionally, it measures the time it takes to launch 
// the application from the initial fork() call and the start of the program. In addition,
// while the application is executing in the child of the first fork() call, fork() is
// called again in the parent of the first fork() call. A delay is added to ensure the
// application is still running, a time measurement is taken, and fork() is called again. 
// In the child of this fork call, which I will call child 2, the application is called 
// again and the the time is taken. With the last two time measurements, the time to call 
// the application in child 2 is calculated. Values are passed to the application in execlp()
// in order for the application to create a unique filename and to have the start time for the time 
// measurement to take place.

// time_gettime(CLOCK_MONTONIC, &foo) is used to capture time values. CLOCK_MONOTONIC
// represents the amount of time in seconds and nanoseconds since an unspecified point in the past
// (for example, system start-up time, or Epoch). This doesn't change after system-start-up time.
// The value of the CLOCK_MONOTONIC clock cannot be set via clock_settime(). 
// From the man page:
// The function clock_getres() finds the resolution (precision) of the specified clock clk_id, 
// and, if res is non-NULL, stores it in the struct timespec pointed to by res. The resolution 
// of clocks depends on the implementation and cannot be configured by a particular process. 
// If the time value pointed to by the argument tp of clock_settime() is not a multiple of res, 
// then it is truncated to a multiple of res.

#include <stdio.h> //for printf
#include <stdint.h> //for uint64 def
#include <stdlib.h> //for exit() def
#include <sys/types.h> //for pid_t
#include <unistd.h> //for fork
#include <sys/time.h> //for gettimeofday
#include <time.h>

#define BILLION 1000000000L

clock_t clock(void);
pid_t fork(void);
int execlp(const char *file, const char *arg, ...);

int main(int argc, char *argv[])
{
	pid_t pid, pid2; //used to identify parent or child process after fork
	uint64_t initialTime, initialTime2; //values used to measure start time before fork() calls
	struct timespec start, start2; //used to convert seconds + nanoseconds into single time unit (in nanoseconds)
	int execFlag; //int value used so ./application is using different file names in each exec() call
	char startString[64]; //stores combined start times as string
	char flag[64]; //stores integer value used to determine file name in ./application
	char *ptr0, *ptr1, *ptr2; //creating char * for passing args to ./application using exec()
	ptr0 = "./application"; //hard coded name of application to be run
	
	if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
		printf("error using clock_gettime. exiting.\n");
		exit(1);
	} //mark start time
	initialTime = BILLION * start.tv_sec + start.tv_nsec; //combined seconds and nanonseconds 
	if (sprintf(startString, "%llu", (long long unsigned int) initialTime) == -1) {
		printf("error converting time to string. exiting. \n");
		exit(1);
	} //startString now stores start time 
	ptr1 = startString; //ptr1 now points to startString

	if ( (pid = fork() ) == -1) { //if -1 is returned, error
		fprintf(stderr,"fork error. exiting.\n");
		exit(1);
	} else if (pid == 0) { //child 1
		execFlag = 1; //used as flag for ./application to choose unique filename
		sprintf(flag, "%d", execFlag); //converts execFlag to string
		ptr2 = flag; 
 		//executes ./application, passes start time and flag
		if ((execlp(ptr0, ptr0, ptr1, ptr2, (char *) NULL)) == -1) { 
			printf("Error in Child 1 Process execlp\n");
			exit(1);
		}
	} else { //parent 1
		sleep(2); //delays for 2 seconds to ensure child 1 is in processing loop
		if(clock_gettime(CLOCK_MONOTONIC, &start2) == -1) {
			printf("error converting time to string in parent. exiting.\n");
			exit(1);
		} //gets new initial time for references in child 2
		initialTime2 = BILLION * start2.tv_sec + start2.tv_nsec; //combines seconds and nanoseconds
		sprintf(startString, "%llu", (long long unsigned int) initialTime2); //startString now stores start time
		if ( (pid2 = fork() ) == -1) { //if -1 is returned, error
			fprintf(stderr,"fork2 error. exiting.\n");
			exit(1);
		} else if (pid2 == 0) { //child 2
			execFlag = 2; //used as flag for ./application to choose unique filename
			sprintf(flag, "%d", execFlag); //converts execFlag to string
			ptr2 = flag;
			//executes ./application, passes start time and flag
			if ((execlp(ptr0, ptr0, ptr1, ptr2, (char *) NULL)) == -1) { 
				printf("Error in Child2 Process execlp\n");
				exit(1);
			}
		} else { //parent 2
			if(wait(NULL) == -1) { //blocks until all children processes finish. 
				printf("error using wait. exiting.\n");
				exit(1);
			} 
		} //end parent 2
	} //end parent 1
} //end main()
