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
#include<string.h>
#include "scheduler.c"
#include "process.c"
#include"os-kernel.h"
time_t t;
pthread_mutex_t M1;
pthread_mutex_t Context = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Current_mutex = PTHREAD_MUTEX_INITIALIZER;

struct PCB;

struct Queue;

int context_switches = 0;

// Array of running process of size 4 , one index for every cpu
struct PCB* running_process[4];


struct Queue* ready_queue;

struct Queue* Arrival_queue;

struct Queue* IO_queue;

double time_count = 0;

char Scheduling_type;
double time_slice_for_RR=-1;
int cpu_count = 0;

int time_slice[4];
int terminated_count = 0;
double avg_waiting_time = 0;

char* output_file_name;

int check_equal(double d1, double d2){
    if(d1-d2 < 0.00001 && d1-d2 > -0.00001){
        return 1;
    }
    return 0;
}

void print_state_of_cpus()
{
        printf("%lf\t", time_count);

        int no_of_process = 0;
        // number of running process
        for(int i = 0; i < 4; i++){
            if(running_process[i] != NULL){
                no_of_process++;
            }
        }
        printf("%d\t", no_of_process);

        int no_in_ready_queue = 0;
        struct PCB*t = ready_queue->front;
        while(t != NULL){
            no_in_ready_queue++;
            t = t->next;
        }
        printf("%d\t", no_in_ready_queue);

        
        // number of prcesses in IO 
        int no_of_IO = 0;
        struct PCB* temp = IO_queue->front;
        while(temp != NULL){
            no_of_IO++;
            temp = temp->next;
        }
        printf("%d\t", no_of_IO);
        
        // //print the process executing on all cpus , if NULL print IDLE
        for(int i = 0; i < 4; i++){
            if(running_process[i] != NULL){
                printf("%s\t", running_process[i]->name);
            }
            else{
                printf("IDLE\t");
            }
        }
        // print IO queue
        printf("<");
        // print IO queue if not empty
        if(isEmpty(IO_queue))
        {
            printf("--");
        }
        if(IO_queue->front != NULL){
            struct PCB* temp = IO_queue->front;
            while(temp != NULL){
                printf("%s ", temp->name);
                temp = temp->next;
            }
        }
        printf(">\t");

        printf("\n");

}


void write_state_of_cpus()
{   
        FILE* fp = fopen(output_file_name, "a");
        fprintf( fp, "%lf\t", time_count);
        // append to line
        

        


        int no_of_process = 0;
        // number of running process
        for(int i = 0; i < 4; i++){
            if(running_process[i] != NULL){
                no_of_process++;
            }
        }
        // printf("%d\t", no_of_process);
        fprintf( fp, "%d\t", no_of_process);


        int no_in_ready_queue = 0;
        struct PCB*t = ready_queue->front;
        while(t != NULL){
            no_in_ready_queue++;
            t = t->next;
        }
        fprintf( fp, "%d\t", no_in_ready_queue);
        
        // number of prcesses in IO 
        int no_of_IO = 0;
        struct PCB* temp = IO_queue->front;
        while(temp != NULL){
            no_of_IO++;
            temp = temp->next;
        }
        // printf("%d\t", no_of_IO);
        fprintf( fp, "%d\t", no_of_IO);
        
        // //print the process executing on all cpus , if NULL print IDLE
        for(int i = 0; i < 4; i++){
            if(running_process[i] != NULL){
                // printf("%s\t", running_process[i]->name);
                fprintf( fp, "%s\t", running_process[i]->name);
            }
            else{
                // printf("IDLE\t");
                fprintf( fp, "IDLE\t");
            }
        }
        // print IO queue
        // /printf("<");
        fprintf(fp,"<");
        // print IO queue if not empty
        if(isEmpty(IO_queue))
        {
            fprintf(fp,"--");
        }
        if(IO_queue->front != NULL){
            struct PCB* temp = IO_queue->front;
            while(temp != NULL){
                fprintf(fp,"%s ", temp->name);
                temp = temp->next;
            }
        }
        fprintf(fp,">\t");
        // printf("\n");
        fprintf( fp, "\n");
        fclose(fp);



}


void Raise_IO_wait_time(){
    // increase in IO time for all IO proceeses by 0.1
        struct PCB* temp = IO_queue->front;
        while(temp != NULL){
            temp->Time_in_IO += 0.1;
            temp = temp->next;
        }
}

int check_end_execution()
{

    if(isEmpty(ready_queue) && isEmpty(Arrival_queue) && isEmpty(IO_queue)){
        if(running_process[0] == NULL && running_process[1] == NULL && running_process[2] == NULL && running_process[3] == NULL){
            return 1;
        }
        return 0;
    }
    else{
        return 0;
    }
}

void check_arrival(){
    // check if any process in Arrival queue has arrival time == time_count
        struct PCB* temp_arrival = Arrival_queue->front;
        while(temp_arrival != NULL){
            if((float)temp_arrival->arrival_time == (float)time_count){
                // create a copy of the process
                struct PCB* copy = copy_PCB(temp_arrival);
                copy->next = NULL;
                // add the process to ready queue
                wakeup(copy);
                // remove the process from arrival queue
                remove_process(Arrival_queue, temp_arrival);
            }
            temp_arrival = temp_arrival->next;
        }

}

void check_if_wakeup()
{
    if(!isEmpty(IO_queue)){
            struct PCB* temp = IO_queue->front;
            while(temp != NULL){
                if((float)temp->Time_in_IO >= 2.0){
                    struct PCB*copy = copy_PCB(temp);
                    remove_process(IO_queue, temp);
                    wakeup(copy);
                    break;
                    temp = temp->next;
                }
                else{
                    temp = temp->next;
                }
            }
        }
}


void check_if_arrived(){
    // check if any process with arrival time <= time_count change state to Ready
        struct PCB* temp = ready_queue->front;
        while(temp != NULL){
            if((float)temp->arrival_time <= (float)time_count){
                temp->state = "Ready";
                temp = temp->next;
            }
            else{
                temp = temp->next;
            }
        }

}


void Increment_Waiting_Time(){
    // increment waiting time for all processes in ready queue
        struct PCB* temp = ready_queue->front;
        while(temp != NULL){
            temp->waiting_time += 0.1;
            temp = temp->next;
        }
}

void Context_Switch(struct PCB* To_run,int timeslice){
    if(To_run == NULL){
        Idle();
        return;
    }
    pthread_mutex_trylock(&Context);
    // change the state of the process
    To_run->state = "Running";
    context_switches++;
    To_run->running_time = 0;
    To_run->time_quantum = timeslice;
    pthread_mutex_unlock(&Context);
}

void* Cpu1(void*arg){
    //printf("CPU1\n");
    while(1){
        //printf("A1");

        while(!time_slice[0]);
        //printf("B1");

        pthread_mutex_trylock(&M1);
        if(isEmpty(ready_queue) && running_process[0] == NULL){
            time_slice[0] = 0;
            pthread_mutex_unlock(&M1);
            continue;
        }
        // check if any process with arrival time == time_count and state = Ready
        if(Scheduling_type == 'p' && running_process[0] != NULL){
            pthread_mutex_trylock(&Current_mutex);
            struct PCB*max = NULL;
            struct PCB*temp1 = ready_queue->front;
            // find the process in ready queue with highest priority
            while(temp1 != NULL){
                if(max == NULL){
                    max = temp1;
                }
                else if(temp1->priority > max->priority){
                    max = temp1;
                }
                temp1 = temp1->next;
            }
            
            if(max && max->priority > running_process[0]->priority)
            {
                preempt(running_process[0]);
                struct PCB* p = remove_process(ready_queue, max);
                Context_Switch(p, -1);
                //p->arrival_time = 100000;
                p->next = NULL;
                p->state = "Running";
                p->start_time = time_count;
                p->running_time = 0;
                running_process[0] = p;
            }
        
            pthread_mutex_unlock(&Current_mutex);
        }
        // check if scheduling type = 'R' and running time >= time_slice_for_RR call preempt
        if(Scheduling_type == 'r' && running_process[0] != NULL && (float)running_process[0]->running_time >= time_slice_for_RR){
            //printf("CPU1 is ideal\n");
            running_process[0]->next = NULL;
            preempt(running_process[0]);
            running_process[0] = NULL;
            running_process[0] = Schedule();
            if(running_process[0]){
                Context_Switch(running_process[0],time_slice_for_RR);
            //printf("%s\n", running_process[0]->name);
                running_process[0]->state = "Running";
            // get system current time in seconds
                running_process[0]->start_time = time_count;
            //running_process[0]->start_time = get_time();
                running_process[0]->running_time = 0;
                }
            time_slice[0] = 0;
            pthread_mutex_unlock(&M1);
            continue;
        }
        if(running_process[0] == NULL)
        {
            running_process[0] = Schedule();
            Context_Switch(running_process[0],time_slice_for_RR);
            //printf("%s\n", running_process[0]->name);
            running_process[0]->state = "Running";
            // get system current time in seconds
            running_process[0]->start_time = time_count;
            //running_process[0]->start_time = get_time();
            running_process[0]->running_time = 0;
            time_slice[0] = 0;
            continue;
            
            
        }
        else{
             // check if the process is done
            if(check_equal(running_process[0]->Cpu_burst_time,0.0)){
                //printf("Terminating process %s with runttime %lf and burst time %lf\n", running_process[0]->name, running_process[0]->running_time, running_process[0]->Cpu_burst_time);
                Terminate(running_process[0]);
                terminated_count++;
                avg_waiting_time += running_process[0]->waiting_time;
                //printf("Process %s terminated after running time %lf\n", running_process[0]->name, running_process[0]->running_time);
                running_process[0] = NULL;
                running_process[0] = Schedule();
                if(running_process[0]){
                Context_Switch(running_process[0],time_slice_for_RR);
            //printf("%s\n", running_process[0]->name);
                running_process[0]->state = "Running";
            // get system current time in seconds
                running_process[0]->start_time = time_count;
            //running_process[0]->start_time = get_time();
                running_process[0]->running_time = 0;
                }
                time_slice[0] = 0;
                pthread_mutex_unlock(&M1);
                continue;
            }
            // if running time has passed I/O time then move the process to IO queue
            if((float)running_process[0]->Total_Cpu_time == (float)running_process[0]->Io_time && running_process[0]->Io_time > 0 && running_process[0]->times_IO_completed < running_process[0]->times_IO){
                running_process[0]->next = NULL;
                running_process[0]->Total_Cpu_time = 0;
                //printf("Process %s moved to IO queue after running time %lf\n", running_process[0]->name, running_process[0]->running_time);
                running_process[0]->times_IO_completed++;
                yield(running_process[0]);
                running_process[0] = NULL;
                running_process[0] = Schedule();
                if(running_process[0]){
                Context_Switch(running_process[0],time_slice_for_RR);
            //printf("%s\n", running_process[0]->name);
                running_process[0]->state = "Running";
            // get system current time in seconds
                running_process[0]->start_time = time_count;
            //running_process[0]->start_time = get_time();
                running_process[0]->running_time = 0;
                }
                time_slice[0] = 0;
                pthread_mutex_unlock(&M1);
                continue;
            }
            //printf("Running process %s with runnin_time %lf and remaining burst time %lf\n", running_process[0]->name, running_process[0]->running_time, running_process[0]->Cpu_burst_time);
            
            if(running_process[0]->Cpu_burst_time > 0){
               // decrement the cpu burst time
                running_process[0]->Cpu_burst_time -= 0.1;
                running_process[0]->running_time += 0.1;
                running_process[0]->Total_Cpu_time += 0.1;
                time_slice[0] = 0;
            }           
        }
        pthread_mutex_unlock(&M1);
    }

    pthread_exit(NULL);
}

void* Cpu2(void*arg){
    //printf("CPU2\n");
    pthread_mutex_trylock(&M1);

while(1){
        //printf("A2");
        
        while(!time_slice[1]);
        //printf("B2");
        pthread_mutex_trylock(&M1);
        
        if(isEmpty(ready_queue) && running_process[1] == NULL){
            time_slice[1] = 0;
            pthread_mutex_unlock(&M1);// IDEAL
            continue;
        }

        // check if any process with arrival time == time_count and state = Ready
        if(Scheduling_type == 'p' && running_process[1] != NULL){
            pthread_mutex_trylock(&Current_mutex);
            struct PCB*max = NULL;
            struct PCB*temp1 = ready_queue->front;
            // find the process in ready queue with highest priority
            while(temp1 != NULL){
                if(max == NULL){
                    max = temp1;
                }
                else if(temp1->priority > max->priority){
                    max = temp1;
                }
                temp1 = temp1->next;
            }
            
            if(max && max->priority > running_process[1]->priority)
            {
                preempt(running_process[1]);
                struct PCB* p = remove_process(ready_queue, max);
                Context_Switch(p, -1);
                //p->arrival_time = 100000;
                p->next = NULL;
                p->state = "Running";
                p->start_time = time_count;
                p->running_time = 0;
                running_process[1] = p;
            }
        
            pthread_mutex_unlock(&Current_mutex);
        }

        // check if scheduling type = 'R' and running time >= time_slice_for_RR call preempt
        if(Scheduling_type == 'r' && running_process[1] != NULL && (float)running_process[1]->running_time >= time_slice_for_RR){
            //printf("CPU1 is ideal\n");
            running_process[1]->next = NULL;
            preempt(running_process[1]);
            running_process[1] = NULL;
           // running_process[0] = Schedule();
            running_process[1] = Schedule();
            if(running_process[1]){
                Context_Switch(running_process[1],time_slice_for_RR);
            //printf("%s\n", running_process[0]->name);
                running_process[1]->state = "Running";
            // get system current time in seconds
                running_process[1]->start_time = time_count;
            //running_process[0]->start_time = get_time();
                running_process[1]->running_time = 0;
                }
            time_slice[1] = 0;
            pthread_mutex_unlock(&M1);
            continue;
            //printf("Time: %lf",time_count);
        }
        
        
        // check if it has a running process
        else if(running_process[1] == NULL)
        {
            running_process[1] = Schedule();
            Context_Switch(running_process[1], time_slice_for_RR);
            //printf("%s\n", running_process[0]->name);
            running_process[1]->state = "Running";
            // get system current time in seconds
            running_process[1]->start_time = time_count;
            //running_process[0]->start_time = get_time();
            running_process[1]->running_time = 0;
            time_slice[1] = 0;
            pthread_mutex_unlock(&M1);
            continue;
            
            
        }
        else{
             // check if the process is done
            if(check_equal(running_process[1]->Cpu_burst_time,0.0)){
               // printf("Terminating process %s with runttime %lf and burst time %lf\n", running_process[1]->name, running_process[0]->running_time, running_process[0]->Cpu_burst_time);
                Terminate(running_process[1]);
                terminated_count++;
                avg_waiting_time += (float)running_process[1]->waiting_time;
                //printf("Process %s terminated after running time %lf\n", running_process[0]->name, running_process[0]->running_time);
                running_process[1] = NULL;
                running_process[1] = Schedule();
                if(running_process[1]){
                Context_Switch(running_process[1],time_slice_for_RR);
            //printf("%s\n", running_process[0]->name);
                running_process[1]->state = "Running";
            // get system current time in seconds
                running_process[1]->start_time = time_count;
            //running_process[0]->start_time = get_time();
                running_process[1]->running_time = 0;
                }
                time_slice[1] = 0;
                continue;
            }
            // if running time has passed I/O time then move the process to IO queue
            if((float)running_process[1]->Total_Cpu_time == (float)running_process[1]->Io_time && running_process[1]->Io_time > 0 && running_process[1]->times_IO_completed < running_process[1]->times_IO){
                running_process[1]->next = NULL;
                running_process[1]->Total_Cpu_time = 0;
                //printf("Process %s moved to IO queue after running time %lf\n", running_process[0]->name, running_process[0]->running_time);
                running_process[1]->times_IO_completed++;
                yield(running_process[1]);
                running_process[1] = NULL;
                running_process[1] = Schedule();
                if(running_process[1]){
                Context_Switch(running_process[1],time_slice_for_RR);
            //printf("%s\n", running_process[0]->name);
                running_process[1]->state = "Running";
            // get system current time in seconds
                running_process[1]->start_time = time_count;
            //running_process[0]->start_time = get_time();
                running_process[1]->running_time = 0;
                }
                time_slice[1] = 0;
                pthread_mutex_unlock(&M1);
                continue;
            }
            //printf("Running process %s with runnin_time %lf and remaining burst time %lf\n", running_process[0]->name, running_process[0]->running_time, running_process[0]->Cpu_burst_time);
            
            if(running_process[1]->Cpu_burst_time > 0){
               // decrement the cpu burst time
                running_process[1]->Cpu_burst_time -= 0.1;
                running_process[1]->running_time += 0.1;
                running_process[1]->Total_Cpu_time += 0.1;
                time_slice[1] = 0;
                pthread_mutex_unlock(&M1);
            }           
        }
        pthread_mutex_unlock(&M1);
    }
    pthread_exit(NULL);

}

void* Cpu3(void*arg){
    pthread_mutex_trylock(&M1);
while(1){
        //printf("A2");
        
        while(!time_slice[2]);
        //printf("B2");
        pthread_mutex_trylock(&M1);
        
        if(isEmpty(ready_queue) && running_process[2] == NULL){
            time_slice[2] = 0;
            pthread_mutex_unlock(&M1);// IDEAL
            continue;
        }

        // check if any process with arrival time == time_count and state = Ready
        if(Scheduling_type == 'p' && running_process[2] != NULL){
            pthread_mutex_trylock(&Current_mutex);
            struct PCB*max = NULL;
            struct PCB*temp1 = ready_queue->front;
            // find the process in ready queue with highest priority
            while(temp1 != NULL){
                if(max == NULL){
                    max = temp1;
                }
                else if(temp1->priority > max->priority){
                    max = temp1;
                }
                temp1 = temp1->next;
            }
            
            if(max && max->priority > running_process[2]->priority)
            {
                preempt(running_process[2]);
                struct PCB* p = remove_process(ready_queue, max);
                Context_Switch(p, -1);
                //p->arrival_time = 100000;
                p->next = NULL;
                p->state = "Running";
                p->start_time = time_count;
                p->running_time = 0;
                running_process[2] = p;
            }
        
            pthread_mutex_unlock(&Current_mutex);
        }

        // check if scheduling type = 'R' and running time >= time_slice_for_RR call preempt
        if(Scheduling_type == 'r' && running_process[2] != NULL && (float)running_process[2]->running_time >= time_slice_for_RR){
            //printf("CPU1 is ideal\n");
            running_process[2]->next = NULL;
            preempt(running_process[2]);
            running_process[2] = NULL;
           // running_process[0] = Schedule();
            running_process[2] = Schedule();
            if(running_process[2]){
                Context_Switch(running_process[2],time_slice_for_RR);
            //printf("%s\n", running_process[0]->name);
                running_process[2]->state = "Running";
            // get system current time in seconds
                running_process[2]->start_time = time_count;
            //running_process[0]->start_time = get_time();
                running_process[2]->running_time = 0;
                }
            time_slice[2] = 0;
            pthread_mutex_unlock(&M1);
            continue;
            //printf("Time: %lf",time_count);
        }
        
        
        // check if it has a running process
        else if(running_process[2] == NULL)
        {
            running_process[2] = Schedule();
            Context_Switch(running_process[2], time_slice_for_RR);
            //printf("%s\n", running_process[0]->name);
            running_process[2]->state = "Running";
            // get system current time in seconds
            running_process[2]->start_time = time_count;
            //running_process[0]->start_time = get_time();
            running_process[2]->running_time = 0;
            time_slice[2] = 0;
            pthread_mutex_unlock(&M1);
            continue;
            
            
        }
        else{
             // check if the process is done
            if(check_equal(running_process[2]->Cpu_burst_time,0.0)){
               // printf("Terminating process %s with runttime %lf and burst time %lf\n", running_process[1]->name, running_process[0]->running_time, running_process[0]->Cpu_burst_time);
                Terminate(running_process[2]);
                terminated_count++;
                avg_waiting_time += (float)running_process[2]->waiting_time;
                //printf("Process %s terminated after running time %lf\n", running_process[0]->name, running_process[0]->running_time);
                running_process[2] = NULL;
                running_process[2] = Schedule();
                if(running_process[2]){
                Context_Switch(running_process[2],time_slice_for_RR);
            //printf("%s\n", running_process[0]->name);
                running_process[2]->state = "Running";
            // get system current time in seconds
                running_process[2]->start_time = time_count;
            //running_process[0]->start_time = get_time();
                running_process[2]->running_time = 0;
                }
                time_slice[2] = 0;
                continue;
            }
            // if running time has passed I/O time then move the process to IO queue
            if((float)running_process[2]->Total_Cpu_time == (float)running_process[2]->Io_time && running_process[2]->Io_time > 0 && running_process[2]->times_IO_completed < running_process[2]->times_IO){
                running_process[2]->next = NULL;
                running_process[2]->Total_Cpu_time = 0;
                //printf("Process %s moved to IO queue after running time %lf\n", running_process[0]->name, running_process[0]->running_time);
                running_process[2]->times_IO_completed++;
                yield(running_process[2]);
                running_process[2] = NULL;
                running_process[2] = Schedule();
                if(running_process[2]){
                Context_Switch(running_process[2],time_slice_for_RR);
            //printf("%s\n", running_process[0]->name);
                running_process[2]->state = "Running";
            // get system current time in seconds
                running_process[2]->start_time = time_count;
            //running_process[0]->start_time = get_time();
                running_process[2]->running_time = 0;
                }
                time_slice[2] = 0;
                pthread_mutex_unlock(&M1);
                continue;
            }
            //printf("Running process %s with runnin_time %lf and remaining burst time %lf\n", running_process[0]->name, running_process[0]->running_time, running_process[0]->Cpu_burst_time);
            
            if(running_process[2]->Cpu_burst_time > 0){
               // decrement the cpu burst time
                running_process[2]->Cpu_burst_time -= 0.1;

                running_process[2]->running_time += 0.1;
                running_process[2]->Total_Cpu_time += 0.1;
                time_slice[2] = 0;
                pthread_mutex_unlock(&M1);
            }           
        }
        pthread_mutex_unlock(&M1);
    }
    pthread_exit(NULL);

}

void* Cpu4(void*arg){
    pthread_mutex_trylock(&M1);
    while(1){
        while(!time_slice[3]);
        //printf("B2");
        pthread_mutex_trylock(&M1);
        
        if(isEmpty(ready_queue) && running_process[3] == NULL){
            time_slice[3] = 0;
            pthread_mutex_unlock(&M1);// IDEAL
            continue;
        }

        // check if any process with arrival time == time_count and state = Ready
        if(Scheduling_type == 'p' && running_process[3] != NULL){
            pthread_mutex_trylock(&Current_mutex);
            struct PCB*max = NULL;
            struct PCB*temp1 = ready_queue->front;
            // find the process in ready queue with highest priority
            while(temp1 != NULL){
                if(max == NULL){
                    max = temp1;
                }
                else if(temp1->priority > max->priority){
                    max = temp1;
                }
                temp1 = temp1->next;
            }
            
            if(max && max->priority > running_process[3]->priority)
            {
                preempt(running_process[3]);
                struct PCB* p = remove_process(ready_queue, max);
                Context_Switch(p, -1);
                //p->arrival_time = 100000;
                p->next = NULL;
                p->state = "Running";
                p->start_time = time_count;
                p->running_time = 0;
                running_process[3] = p;
            }
        
            pthread_mutex_unlock(&Current_mutex);
        }

        // check if scheduling type = 'R' and running time >= time_slice_for_RR call preempt
        if(Scheduling_type == 'r' && running_process[3] != NULL && (float)running_process[3]->running_time >= time_slice_for_RR){
            //printf("CPU1 is ideal\n");
            running_process[3]->next = NULL;
            preempt(running_process[3]);
            running_process[3] = NULL;
           // running_process[0] = Schedule();
            running_process[3] = Schedule();
            if(running_process[3]){
                Context_Switch(running_process[3],time_slice_for_RR);
            //printf("%s\n", running_process[0]->name);
                running_process[3]->state = "Running";
            // get system current time in seconds
                running_process[3]->start_time = time_count;
            //running_process[0]->start_time = get_time();
                running_process[3]->running_time = 0;
                }
            time_slice[3] = 0;
            pthread_mutex_unlock(&M1);
            continue;
            //printf("Time: %lf",time_count);
        }
        
        
        // check if it has a running process
        else if(running_process[3] == NULL)
        {
            running_process[3] = Schedule();
            Context_Switch(running_process[3], time_slice_for_RR);
            //printf("%s\n", running_process[0]->name);
            running_process[3]->state = "Running";
            // get system current time in seconds
            running_process[3]->start_time = time_count;
            //running_process[0]->start_time = get_time();
            running_process[3]->running_time = 0;
            time_slice[3] = 0;
            pthread_mutex_unlock(&M1);
            continue;
            
            
        }
        else{
             // check if the process is done
            if(check_equal(running_process[3]->Cpu_burst_time,0.0)){
               // printf("Terminating process %s with runttime %lf and burst time %lf\n", running_process[1]->name, running_process[0]->running_time, running_process[0]->Cpu_burst_time);
                Terminate(running_process[3]);
                terminated_count++;
                avg_waiting_time += (float)running_process[3]->waiting_time;
                //printf("Process %s terminated after running time %lf\n", running_process[0]->name, running_process[0]->running_time);
                running_process[3] = NULL;
                running_process[3] = Schedule();
                if(running_process[3]){
                Context_Switch(running_process[3],time_slice_for_RR);
            //printf("%s\n", running_process[0]->name);
                running_process[3]->state = "Running";
            // get system current time in seconds
                running_process[3]->start_time = time_count;
            //running_process[0]->start_time = get_time();
                running_process[3]->running_time = 0;
                }
                time_slice[3] = 0;
                continue;
            }
            // if running time has passed I/O time then move the process to IO queue
            if((float)running_process[3]->Total_Cpu_time == (float)running_process[3]->Io_time && running_process[3]->Io_time > 0 && running_process[3]->times_IO_completed < running_process[3]->times_IO){
                running_process[3]->next = NULL;
                running_process[3]->Total_Cpu_time = 0;
                //printf("Process %s moved to IO queue after running time %lf\n", running_process[0]->name, running_process[0]->running_time);
                running_process[3]->times_IO_completed++;
                yield(running_process[3]);
                running_process[3] = NULL;
                running_process[3] = Schedule();
                if(running_process[3]){
                Context_Switch(running_process[3],time_slice_for_RR);
            //printf("%s\n", running_process[0]->name);
                running_process[3]->state = "Running";
            // get system current time in seconds
                running_process[3]->start_time = time_count;
            //running_process[0]->start_time = get_time();
                running_process[3]->running_time = 0;
                }
                time_slice[3] = 0;
                pthread_mutex_unlock(&M1);
                continue;
            }
            //printf("Running process %s with runnin_time %lf and remaining burst time %lf\n", running_process[0]->name, running_process[0]->running_time, running_process[0]->Cpu_burst_time);
            
            if(running_process[3]->Cpu_burst_time > 0){
               // decrement the cpu burst time
                running_process[3]->Cpu_burst_time -= 0.1;
                /*
                if(running_process[3]->Cpu_burst_time < 0){
                    running_process[3]->Cpu_burst_time = 0;
                    Terminate(running_process[3]);
                    terminated_count++;
                    avg_waiting_time += (float)running_process[3]->waiting_time;
                    running_process[3] = NULL;
                    time_slice[3] = 0;
                    pthread_mutex_unlock(&M1);
                    continue;
                }
                */
                
                running_process[3]->running_time += 0.1;
                running_process[3]->Total_Cpu_time += 0.1;
                time_slice[3] = 0;
                pthread_mutex_unlock(&M1);
            }           
        }
        pthread_mutex_unlock(&M1);
    }
    pthread_exit(NULL);
}
// read the input file for processes
void read_file_processes1(char*file_name)
{
    char name[50];
    int priority;
    double arrival_time;
    char Process_type;
    double Cpu_burst_time;
    int Io_time;
    struct PCB* p;
    int pid=0;
    FILE* fp = fopen(file_name, "r");
    if(fp == NULL){
        printf("Error opening file\n");
        exit(1);
    }
    // read the first line 
    char line[100];
    fgets(line, 100, fp);
    // count number of spaces in the line
    int num_spaces = 1;
    for(int i = 0; i < strlen(line); i++){
        if(line[i] == '\t'){
            num_spaces++;
        }
    }
    srand((unsigned) time(&t));
    //printf("Number of spaces in the line: %d\n", num_spaces);
    if(num_spaces == 6){
    // columns are seperated by uneven spaces
    while(fscanf(fp, "%s %d %lf %c %lf %d", name, &priority, &arrival_time, &Process_type, &Cpu_burst_time, &Io_time) == 6){
        // create a new process
        p = (struct PCB*)malloc(sizeof(struct PCB));
        p->pid = pid;
        p->name = (char*)malloc(sizeof(char)*50);
        strcpy(p->name, name);
        p->priority = priority;
        p->arrival_time = arrival_time;
        p->Process_type = Process_type;
        p->Cpu_burst_time = Cpu_burst_time;
        p->times_IO = Io_time; // no. of times process goes to IO
        // time at which process goes to IO needs to be  <= Cpu_burst_time/times_IO
        p->Io_time = rand()%(int)(Cpu_burst_time/Io_time);
        while((float)p->Io_time <= 0.0){
            p->Io_time = rand()%(int)(Cpu_burst_time/Io_time);
        }
        //printf("Random value of IO is %lf\n", p->Io_time);
        p->times_IO_completed = 0;
        // p->Io_time = Io_time;
        p->next = NULL;
        p->state = "New";
        // add the process to the Arrival queue
        enqueue(Arrival_queue, p);
        pid++;
    }
    }
    else{
        while(fscanf(fp, "%s %d %lf %c", name, &priority, &arrival_time, &Process_type) == 4){
        // create a new process
        p = (struct PCB*)malloc(sizeof(struct PCB));
        p->pid = pid;
        p->name = (char*)malloc(sizeof(char)*50);
        strcpy(p->name, name);
        p->priority = priority;
        p->arrival_time = arrival_time;
        p->Process_type = Process_type;
        

        p->Cpu_burst_time = rand()%10 +2;
	if(Process_type == 'C') { p->times_IO = -1;}
	else {
        	p->times_IO = (rand()%5)+1; 
	}
        // no. of times process goes to IO
        // time at which process goes to IO needs to be  <= Cpu_burst_time/times_IO
        p->Io_time = rand()%2;
        while((float)p->Io_time <= 0.0){
            p->Io_time = rand()%2;
        }
        // printf("Random value of IO is %lf\n", p->Io_time);
        // printf("Random CPU burst time is %lf\n", p->Cpu_burst_time);
        // printf("Number of times IO is %d\n", p->times_IO);
        //printf("Random value of IO is %lf\n", p->Io_time);
        p->times_IO_completed = 0;
        // p->Io_time = Io_time;
        p->next = NULL;
        p->state = "New";
        // add the process to the Arrival queue
        enqueue(Arrival_queue, p);
        pid++;
    }

    }

    fclose(fp);
}

void* controllerThread(void*Arg){
        pthread_t cpu1;
        pthread_t cpu2;
        pthread_t cpu3;
        pthread_t cpu4;
        // give time slice = 0
        for(int i=0;i<4; i++){
            time_slice[i] = 0;
        }
    
    printf("Time\t\tRu\tRe\tWa\tCpu1\tCpu2\tCpu3\tCpu4\tI/O queue\n");
    // write in the output file
    FILE* fp = fopen(output_file_name, "w");
    fprintf(fp, "Time\t\tRu\tRe\tWa\tCpu1\tCpu2\tCpu3\tCpu4\tI/O queue\n");
    fclose(fp);

    if(cpu_count == 1){
        pthread_create(&cpu1, NULL, Cpu1, NULL);

        while(!check_end_execution())
        {
            print_state_of_cpus();
            write_state_of_cpus();
            //printQueue(ready_queue);
            printf("\n");
            check_arrival();
            check_if_wakeup();
            Raise_IO_wait_time();
            Increment_Waiting_Time();
            time_slice[0]++;
            while(time_slice[0]);// spinning
            time_count+=0.1;
        }
        printf("Final State of the system\n");
        print_state_of_cpus();
        printf("Total execution Time: %lf seconds\n", time_count);
        printf("Number of Context Switches: %d\n", context_switches);
        printf("Total Time in Ready state for all processes: %lf seconds\n", avg_waiting_time);

    }
    else if(cpu_count == 2){
        pthread_create(&cpu1, NULL, Cpu1, NULL);
        pthread_create(&cpu2, NULL, Cpu2, NULL);
        while(!check_end_execution())
        {
         //   printf("Starting at time %lf\n", time_count);
            print_state_of_cpus();
            write_state_of_cpus();
           // printf("Starting11111");
            check_arrival();
            check_if_wakeup();
            printf("\n");
            //printQueue(ready_queue);
            //printf("\n");
            Raise_IO_wait_time();
            Increment_Waiting_Time();
            time_slice[0]++;
            while(time_slice[0]){;}
         //   printf("Outt1");
            time_slice[1]++;
            while(time_slice[1]){;}
           // printf("Outt2\n");
            time_count+=0.1;
        }
        printf("Final State of the system\n");
        print_state_of_cpus();
        printf("Total execution Time: %lf seconds\n", time_count);
        printf("Number of Context Switches: %d\n", context_switches);
        printf("Total Time in Ready state for all processes: %lf seconds\n", avg_waiting_time);

    }
    else if(cpu_count == 4){
        pthread_create(&cpu1, NULL, Cpu1, NULL);
        pthread_create(&cpu2, NULL, Cpu2, NULL);
        pthread_create(&cpu3, NULL, Cpu3, NULL);
        pthread_create(&cpu4, NULL, Cpu4, NULL);
        while(!check_end_execution())
        {
         //   printf("Starting at time %lf\n", time_count);
            print_state_of_cpus();
            write_state_of_cpus();
           // printf("Starting11111");
            check_arrival();
            check_if_wakeup();
            printf("\n");
            //printQueue(ready_queue);
            //printf("\n");
            Raise_IO_wait_time();
            Increment_Waiting_Time();
            time_slice[0]++;
            while(time_slice[0]){;}
         //   printf("Outt1");
            time_slice[1]++;
            while(time_slice[1]){;}
           // printf("Outt2\n");
            time_slice[2]++;
            while(time_slice[2]){;}
            time_slice[3]++;
            while(time_slice[3]){;}
            time_count+=0.1;
        }
        printf("Final State of the system\n");
        print_state_of_cpus();
        printf("Total execution Time: %lf seconds\n", time_count);
        printf("Number of Context Switches: %d\n", context_switches);
        printf("Total Time in Ready state for all processes: %lf seconds\n", avg_waiting_time);
    }

}

void StartKernel(char*filename, int cpu_count1,char algo,double time_slice)
{
    // open and truncate output file
    FILE *fp;
    fp = fopen(output_file_name, "w");
    fclose(fp);
    ready_queue = (struct Queue*)malloc(sizeof(struct Queue));
    init(ready_queue);
    IO_queue = (struct Queue*)malloc(sizeof(struct Queue));
    init(IO_queue);
    Arrival_queue = (struct Queue*)malloc(sizeof(struct Queue));
    init(Arrival_queue);
    
    read_file_processes1(filename);
    printQueue(Arrival_queue);
    for(int i = 0; i < 4; i++){
        running_process[i] = NULL;
    }
    cpu_count = cpu_count1;
    // create the controller thread
    pthread_t controller;
    pthread_create(&controller, NULL, controllerThread, NULL);
    pthread_join(controller, NULL);
}


int main(int argc,char* argv[])
{

    // read command line arguments
    // <Input file> <# CPUs> r <timeslice> <Output file>
    // if r is present then read timeslice then output file
    // if r is not present then read output file next

    int cpu_count;
    char* input_file;
    char* output_file;
    double time_slice;
    char Scheduling_Algorithm;

    if(argc == 6)
    {
        input_file = argv[1];
        cpu_count = atoi(argv[2]);
        Scheduling_Algorithm = argv[3][0];
        time_slice = atof(argv[4]);
        output_file = argv[5];

    }
    else if (argc == 5)
    {
        input_file = argv[1];
        cpu_count = atoi(argv[2]);
        Scheduling_Algorithm = argv[3][0];
        time_slice = -1;
        output_file = argv[4];
    }
    

    printf("Input file: %s\n", input_file);
    printf("CPU count: %d\n", cpu_count);
    printf("Scheduling Algorithm: %c\n", Scheduling_Algorithm);
    printf("Time slice: %lf\n", time_slice);
    printf("Output file: %s\n", output_file);

    Scheduling_type = Scheduling_Algorithm;
    time_slice_for_RR = time_slice/10;
    output_file_name = output_file;
    StartKernel(input_file, cpu_count, Scheduling_Algorithm, time_slice);

    
    //read_file(input_file);
    //printQueue(Arrival_queue);
    



    



}

#endif
