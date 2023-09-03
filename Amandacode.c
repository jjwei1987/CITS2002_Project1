#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for fork() function


// Arrays: devices[MAX_DEVICES], processes[MAX_RUNNING_PROCESSES], commands[MAX_COMMANDS],
// Set up struct same as commands array: ready_queue[MAX_COMMANDS], running[1], sleep_blocked_queue[], IO_blocked_queue[], parent_blocked_queue[]

// int counter --> keeps a running count of the total time taken to complete the emulation
// int cpu_counter --> keeps a running count of the current processes CPU executing process
// int total_cpu_counter --> keeps a running count of the total time the CPU was executing processes
// int cpu_utilisation --> the percentage of time that the CPU is executing processes

/* counter var increases with every function that uses usecs, will need to decide how to track given processes can be in the blocked queues while other processes are running. Dont want to increase the counter 2x while processes are passing time in the blocked queue and processes are running. BUT also want to increase the counter when no processes are running but time is passing in the blocked_queues */

// cpu_counter only increases when the "running" function is implemented.


-----------

// Define arrays commands[MAX_COMMANDS], devices[MAX_DEVICES] and processes[MAX_RUNNING_PROCESSES]
// Define a structure for devices
struct Device {
    char name[MAX_DEVICE_NAME];
    int read_speed;
    int write_speed;
};

// Define a structure for syscalls within a command
struct Syscall {
    int execute;
    char name[MAX_COMMAND_NAME];
    int value1;
    int value2;
};

// Define a structure for commands
struct Command {
    char command_name[MAX_COMMAND_NAME];
    struct Syscall syscalls[MAX_SYSCALLS_PER_PROCESS];
};

// Create arrays of devices and commands
struct Device devices[MAX_DEVICES];
struct Command commands[MAX_COMMANDS];


//Time Quantum: if counter % DEFAULT_TIME_QUANTUM == 0 then we want to stop process execution, put process back in ready_queue until it can run again.

void initialiseQueues() {
    // Initialise queues for Ready, Running, and Blocked processes
}

void addToReadyQueue(/* Parameters */) {
    //  Append all commands to the ready_queue array
}


void moveFromReadyToRunning() {
    // Move a process from ready_queue to running
}
/*
    Loop through ready_queue
    append i to running, del i from ready_queue
    add 5 to counter
 */


void executeProcess(/* Parameters */) {
    // Implement the main logic for process execution
}
/*  while loop for the time quantum
    if statement for process exit if cpu_counter == total expected execution time
    else, add execute time to the counter and cpu_counter
 */

void handleSystemCall(/* Parameters */) {
    // Implement logic to handle system calls (e.g., sleep, read, write, etc.)
}


void addToBlockedQueue(/* Parameters */) {
    // Add a process to the appropriate blocked queue (sleep_blocked_queue, IO_blocked_queue, parent_blocked_queue)
}

void moveToReadyQueue(/* Parameters */) {
    // Move a process from a blocked queue to the ready_queue
}

/* sleep_blocked_queue[], append here if sleep syscall
 add 10 to the counter for the move to blocked_queue
 consider adding sleep time to the counter,
 append to ready_queue and add 10 to counter.

 IO_blocked_queue[], append here for I/O read and write syscalls
 add 10 to the counter for the move to blocked_queue
 ....
 append to ready_queue and add 10 to counter.
 
 parent_blocked_queue[], append here for parent process waiting for child process to terminate
 add 10 to the counter for the move to blocked_queue
 ....
 append to ready_queue and add 10 to counter.
 */





