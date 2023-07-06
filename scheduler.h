#ifndef Scheduler_H
#define Scheduler_H
#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/wait.h>
#include<string.h>
#include<time.h>
#include "process.h"

struct Queue{
    struct PCB* front;
    struct PCB* rear;
};



void init(struct Queue* q);
int isEmpty(struct Queue* q);
void enqueue(struct Queue* q, struct PCB* p);
struct PCB* dequeue(struct Queue* q);
struct PCB* remove_process(struct Queue*q, struct PCB* p);
void printQueue(struct Queue* q);
struct PCB* Schedule();
void yield(struct PCB* current);
void Terminate(struct PCB* current);
void wakeup(struct PCB* current);
void preempt(struct PCB* current);



#endif
