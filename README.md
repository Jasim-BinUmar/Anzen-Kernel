The project consists of the following file structure:

#OS-Kernel.h/c
#Contains the following function prototypes:
• void print_state_of_cpus();
    To print the status of all CPU’s running at a particular time and also the number 
    of processes running, ready, and blocked in IO in kernel.
• void Raise_IO_wait_time();
    To increase the wait time of all the processes in the block queue currently in IO.
• int check_end_execution();
    To check if the kernel needs to terminate (when all the processes have executed).
• void check_arrival();
    To check if a process has arrived in the ready queue. If arrived call wakeup.
• void check_if_wakeup()
    To check if a process has completed its IO time or not. If completed call wakeup.
• void Increment_Waiting_Time();
    To increase the wait time of all the processes in the ready queue that are not 
    running.
• void Context_Switch(struct PCB* To_run,int timeslice);
    To context switch to a running process selected by the kernel.
#Following functions simulate the running of CPUs.
    • void* Cpu1(void*arg);
    • void* Cpu2(void*arg);
    • void* Cpu3(void*arg);
    • void* Cpu4(void*arg);
    • void* controllerThread(void*Arg);
To simulate the controller thread.
  • void read_file_processes1(char*file_name);
      To read data of processes from the input file and save it in PCB.
  • void StartKernel(char*filename, int cpu_count1,char algo,double time_slice)
      To initialize the kernel at the start of execution by loading up the processes and 
      creating threads.

#Processes.h/c:
  It contains the following:
    • struct PCB {};
  Contains all the information regarding a particular process.
    • void P_innit(struct PCB* p);
  Used to initialize a new process when it is created.
    • struct PCB* copy_PCB(struct PCB* p);
    Used to copy an existing process to a new PCB.

#Scheduler.h/c:
  It contains the following:
    • struct Queue{}
      A linkedlist based queue data structure used by the kernel.
    • void init(struct Queue* q);
      A function to initialize a new Queue.
    • int isEmpty(struct Queue* q)
      To check if a queue is Empty or not.
    • void enqueue (struct Queue* q, struct PCB* p)
      To enqueue a process in the respective queue.
    • struct PCB* dequeue(struct Queue* q);
      To dequeue a process from a respective queue.
    • struct PCB* remove_process(struct Queue*q, struct PCB* p)
      To remove a given process from a queue.
    • void printQueue(struct Queue* q)
      To print information of processes currently in queue.
    • struct PCB* Schedule ()
      To schedule a process onto the CPU.
    • void yield (struct PCB* current)
      To run the IO for any process and send it to block queue.
    • void Idle ()
      Gives control back to controller thread until signaled.
    • void Terminate (struct PCB* current);
      To terminate a process.
    • void wakeup (struct PCB* current);
      To wakeup a process ready to run.
    • void preempt (struct PCB* current);
      To take CPU from a process if its time slice expires.
    • void force_preempt(struct PCB* current);
      To take CPU from a process before its time slice expires
#Input File:
  The input file provide to the program contains the information on the processes that are to be 
  scheduled onto the kernel.
#Output File:
  The output file contains the outcome of the kernel after scheduling the given processes onto the 
  CPUS
