#ifndef Process_H
#define Process_H
#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/wait.h>
#include<string.h>
#include<time.h>


struct PCB{
    int pid;
    char* name;
    int priority;
    double arrival_time;
    char Process_type;
    double Cpu_burst_time;
    double Io_time;
    struct PCB* next;
    char*state;
    double start_time;
    double end_time;
    double running_time;
    double Time_in_IO;
    double Total_Cpu_time;
    int time_quantum;
    int times_IO_completed;
    int times_IO;
    double waiting_time;


    
};

void P_innit(struct PCB* p);

struct PCB* copy_PCB(struct PCB* p);




#endif
