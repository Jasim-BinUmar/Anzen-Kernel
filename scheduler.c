#ifndef Scheduler_h
#define Scheduler_h


#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/wait.h>
#include<string.h>
#include "scheduler.h"
pthread_mutex_t M2;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
struct PCB;
struct Queue* ready_queue;
struct Queue* IO_queue;
struct Queue* Arrival_queue;
char Scheduling_type;
// initialize the queue
void init(struct Queue* q){
    q->front = NULL;
    q->rear = NULL;
}
// check if the queue is empty
int isEmpty(struct Queue* q){
    return q->front == NULL;
}

void enqueue_sorted(struct Queue*q, struct PCB*p)
{
    // enqueue the process by order of priority
    if(isEmpty(q))
    {
        p->next = NULL;
        q->front = p;
        q->rear = p;
        return;

    }
    else
    {
        struct PCB* temp = q->front;
        struct PCB* prev = NULL;
        while(temp != NULL && temp->priority >= p->priority)
        {
            prev = temp;
            temp = temp->next;
        }
        if(prev == NULL)
        {
            p->next = q->front;
            q->front = p;
        }
        else
        {
            prev->next = p;
            p->next = temp;
        }
    }
}

// add a process to the queue
void enqueue(struct Queue* q, struct PCB* p){

    if(Scheduling_type == 'p' && q == ready_queue)
    {
        enqueue_sorted(q, p);
        return;
    }
    p->next = NULL;
    if(isEmpty(q)){
        q->front = p;
        q->rear = p;
    }
    else{
        q->rear->next = p;
        q->rear = p;
    }
}




// remove a process from the queue
struct PCB* dequeue(struct Queue* q){
    if(isEmpty(q)){
        return NULL;
    }
    else{
        struct PCB* p = q->front;
        q->front = q->front->next;
        return p;
    }
}

// remove a process from the queue
struct PCB* remove_process(struct Queue*q, struct PCB* p){
    if(isEmpty(q)){
        return NULL;
    }
    else{
        struct PCB* temp = q->front;
        struct PCB* prev = NULL;
        while(temp != NULL){
            if(temp == p){
                if(prev == NULL){
                    q->front = temp->next;
                }
                else{
                    prev->next = temp->next;
                }
                return temp;
            }
            prev = temp;
            temp = temp->next;
        }
        return NULL;
    }
}

// print the queue
void printQueue(struct Queue* q){
    struct PCB* p = q->front;
    while(p != NULL){
        // print the procces
        printf("%d %s %d %lf %c %lf %lf %s\n", p->pid, p->name, p->priority, p->arrival_time, p->Process_type, p->Cpu_burst_time, p->Io_time,p->state);

        p = p->next;
    }
}



// struct PCB* Schedule(struct PCB*current,char type){
//     pthread_mutex_trylock(&M2);

//     struct PCB* next_process = dequeue(ready_queue);
//     return next_process;
//     pthread_mutex_unlock(&M2);
// }

struct PCB* Schedule(){
    // if queue is empty return NULL
    if(isEmpty(ready_queue)){
        return NULL;
    }
    // else return next process
    struct PCB* next_process = dequeue(ready_queue);
    return next_process;
}

void yield(struct PCB* current){
    // process state changed to Waiting
    current->next = NULL;
    current->state = "Waiting";
    current->Time_in_IO = 0;
    // running time = 0
    current->running_time = 0;
    // enqueue to IO queue
    enqueue(IO_queue,current);
}

void Terminate(struct PCB* current){
    // process state changed to Terminated
    current->next = NULL;
    current->state = "Terminated";
    current->end_time = current->start_time + current->running_time;
}

void wakeup(struct PCB* current){
    // A process in the IO queue is woken up
    // process state changed to Ready
    //printf("Going to wake up %s\n", current->name);
    current->next = NULL;
    current->state = "Ready";
    // enqueue to ready queue
   // printf("Remaining CPU time for %s is %lf\n", current->name, current->Cpu_burst_time);
    enqueue(ready_queue,current);
    // signal the condition
    pthread_cond_signal(&cond1);
}

void preempt(struct PCB* current){
    // A process in the ready queue is preempted
    // process state changed to Ready
    current->next = NULL;
    current->state = "Ready";
    current->running_time = 0;
    // enqueue to ready queue
    enqueue(ready_queue,current);
}

void force_preempt(struct PCB* current){
    // For round robin or priority scheduling, a process in the ready queue is preempted
    // process state changed to Ready
    current->next = NULL;
    current->state = "Ready";
    current->running_time = 0;
    // enqueue to ready queue
    enqueue(ready_queue,current);
}

void Idle()
{
    
    if(!isEmpty(IO_queue) || !isEmpty(Arrival_queue) ){
        pthread_mutex_trylock(&M2);
        // wait for the condition signalled by wakeup
        printf("waiting for the condition\n");
        pthread_cond_wait(&cond1, &M2);
        pthread_mutex_unlock(&M2);
    }
    

}


#endif
