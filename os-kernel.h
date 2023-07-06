#ifndef kerenel_H
#define kerenel_H

#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/wait.h>
#include<string.h>
#include<time.h>
#include "scheduler.c"
#include "process.c"

int check_equal(double d1, double d2);
void print_state_of_cpus();
void Raise_IO_wait_time();
int check_end_execution();
void check_arrival();
void check_if_wakeup();
void check_if_arrived();
void Increment_Waiting_Time();
void Context_Switch(struct PCB* To_run,int timeslice);
void* Cpu1(void*arg);
void* Cpu2(void*arg);
void* Cpu3(void*arg);
void* Cpu4(void*arg);
void read_file_processes1(char*file_name);
void read_file_processes2(char*file_name);
void read_file(char*file_name);
void* controllerThread(void*Arg);
void StartKernel(char*filename, int cpu_count1,char algo,double time_slice);


#endif
