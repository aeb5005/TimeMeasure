# Measuring time to fork and execute in C

This program measures the time it takes to fork and exec a process. 

Compile Application.c by the following: 'gcc -o application application.c -lrt'
Compile Timer.c by the following: 'gcc -o timer timer.c -lrt'

Compiled with 'gcc version 4.8.4 (Ubuntu 4.8.4-2ubuntu1~14.04)'

SIZE macro in application.c is currently set to 20000. On my system, CPU: Intel i5-4670k, 8 GB ram, it takes ~ 5 seconds to run ./timer. If necessary, adjust this macro to increase or decrease run time. 

