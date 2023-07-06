#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/wait.h>
#include<string.h>
#include<time.h>
#include "process.h"


void P_innit(struct PCB* p)
{
        p->pid = -1;
        p->name = NULL;
        p->priority = -1;
        p->arrival_time = -1;
        p->Process_type = '\0';
        p->Cpu_burst_time = -1;
        p->Io_time = -1;
        p->next = NULL;
        p->state = NULL;
        p->start_time = -1;
        p->end_time = -1;
        p->running_time = -1;
        p->Time_in_IO = -1;
        p->time_quantum = -1;
        p->times_IO_completed = 0;
        p->times_IO = 0;
        p->waiting_time = 0;
        p->Total_Cpu_time = 0;
}

// a copy constructor
struct PCB* copy_PCB(struct PCB* p){
    struct PCB* new_p = (struct PCB*)malloc(sizeof(struct PCB));
    new_p->pid = p->pid;
    new_p->name = p->name;
    new_p->priority = p->priority;
    new_p->arrival_time = p->arrival_time;
    new_p->Process_type = p->Process_type;
    new_p->Cpu_burst_time = p->Cpu_burst_time;
    new_p->Io_time = p->Io_time;
    new_p->next = p->next;
    new_p->state = p->state;
    new_p->start_time = p->start_time;
    new_p->end_time = p->end_time;
    new_p->running_time = p->running_time;
    new_p->Time_in_IO = p->Time_in_IO;
    new_p->time_quantum = p->time_quantum;
    new_p->times_IO_completed = p->times_IO_completed;
    new_p->times_IO = p->times_IO;
    new_p->waiting_time = p->waiting_time;
    new_p->Total_Cpu_time = p->Total_Cpu_time;
    return new_p;
}


