#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


//  CITS2002 Project 1 2023
//  Student1:   21022228    Jie Wei
//  Student2:   21143691    Amanda Brown
//  Date:       2023-09-15

//  THESE CONSTANTS DEFINE THE MAXIMUM SIZE OF sysconfig AND command DETAILS
//  THAT YOUR PROGRAM NEEDS TO SUPPORT.  YOU'LL REQUIRE THESE //  CONSTANTS
//  WHEN DEFINING THE MAXIMUM SIZES OF ANY REQUIRED DATA STRUCTURES.

#define MAX_DEVICES                     4
#define MAX_DEVICE_NAME                 20
#define MAX_COMMANDS                    10
#define MAX_COMMAND_NAME                20
#define MAX_SYSCALLS_PER_PROCESS        40
#define MAX_RUNNING_PROCESSES           50

//  NOTE THAT DEVICE DATA-TRANSFER-RATES ARE MEASURED IN BYTES/SECOND,
//  THAT ALL TIMES ARE MEASURED IN MICROSECONDS (usecs),
//  AND THAT THE TOTAL-PROCESS-COMPLETION-TIME WILL NOT EXCEED 2000 SECONDS
//  (SO YOU CAN SAFELY USE 'STANDARD' 32-BIT ints TO STORE TIMES).

#define DEFAULT_TIME_QUANTUM            100

#define TIME_CONTEXT_SWITCH             5
#define TIME_CORE_STATE_TRANSITIONS     10
#define TIME_ACQUIRE_BUS                20


//  ----------------------------------------------------------------------

#define CHAR_COMMENT                    '#'     //  All text from, and including, a '#' character up to the end of line is a comment
#define BUFSIZE                         1024    //  The maximum length of any command

//  ----------------------------------------------------------------------
//  GLOBAL VARIABLES
int GLOBAL_CLOCK = 0;                              //  The simulated time, initially 0, always increasing
int TOTAL_PROCESS_TIME_ON_CPU = 0;                 //  The total time that all processes have spent on the CPU
int ndevices = 0;                                  //  The number of I/O devices
int ncommands = 0;                                 //  The number of commands
int nprocesses = 0;                                //  The number of processes that have been created
int nsyscalls = 0;                                 //  The number of system calls that have been made
int time_quantum = DEFAULT_TIME_QUANTUM;                                  //  The time quantum

//  ----------------------------------------------------------------------
// STRUCTURES

// Define a structure to represent a device with its name, read speed, and write speed.
typedef struct {
    char        device_name[MAX_DEVICE_NAME];
    int         read_speed;
    int         write_speed;
} device;

// Create an array of 'device' structures to store device information.
device devices[MAX_DEVICES];

// Define a structure to represent a system call (syscall) with run time, syscall name, device name, argument, and child command name.
typedef struct {
    int         run_time;
    char        syscall_name[MAX_COMMAND_NAME];
    char        device_name[MAX_DEVICE_NAME];
    int         argument;
    char        child_command_name[MAX_COMMAND_NAME];
} sys_call;


sys_call current_syscall;

// Define a structure to represent a command with when it was evoked, name, number of syscalls, and an array of syscalls.
typedef struct {
    int        when_evoked;
    char       command_name[MAX_COMMAND_NAME];
    int        nsyscalls;
    sys_call   sys_calls[MAX_SYSCALLS_PER_PROCESS];
} command;

// Create an array of 'command' structures to store command information.
command command_list[MAX_COMMANDS];

// Define a structure for a queue to indicate when we have commands that need to run
typedef struct {
    char name[MAX_COMMAND_NAME];
    int front;
    int end;
    command processes[MAX_RUNNING_PROCESSES];
} queue;


//Create instances of 'queue' to represent ready, running, and blocked queues.
queue ready_queue;
queue running_queue;
queue blocked_queue;


// ----------------------------------------------------------------------
//  QUEUE AND FUNCTIONS

void init_queue(queue *q) {
    q->front = -1;
    q->end = -1;
}

//check if the queue is empty
int is_queue_empty(const queue *q)
{
    return (q->front == -1);
}

// check if the queue is full
int is_queue_full(queue *q)
{
return ((q->end + 1) % MAX_RUNNING_PROCESSES == q->front);
}


// Add a command to the processes queue.
void add_to_queue(queue *q, command *process) {
    if (is_queue_full(q)) {
        printf("Queue is full\n");// Queue already full
        
    } else {
        if (q->front == -1) {
            q->front = 0;
        }
        q->end = (q->end + 1) % MAX_RUNNING_PROCESSES;
        q->processes[q->end] = *process; // Added process to queue
        printf("Command %s added to ready queue\n", process->command_name); // Success
    }
}

void moveFromReadyToRunning(/*parameters*/)
{
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

// ----------------------------------------------------------------------
// FUNCTIONS TO READ THE SYSTEM CONFIGURATION FILE AND COMMAND FILE

void read_sysconfig(char argv0[], char filename[])
{
    FILE *sysconfig = fopen(filename, "r");

    if (sysconfig != NULL)
    {
        char sys_line[BUFSIZE];
        while (fgets(sys_line, BUFSIZE, sysconfig) != NULL)
        {
            // Check if the line is a comment and skip it if it is.
            if (sys_line[0] == CHAR_COMMENT)
            {
                continue;
            }
            
            // Check if the line starts with 't', indicating it's the line with the time quantum value.
            if (sys_line[0] == 't')
            {
                // Parse and store the time quantum value.
                if (sscanf(sys_line, "%*s %i", &time_quantum) == 1)
                {
                    printf("Time Quantum: %i\n", time_quantum);
                }
                continue; // Skip processing the rest of the loop for this line
            }
            
            // Process other lines (device information) here
            char device_name[MAX_DEVICE_NAME];
            int read_speed;
            int write_speed;
            // Parse device name, read speed, and write speed, and store them in the devices array.
            // Skip the first string in the line, as it's not needed.
            // Devices array size is 3.
            if (sscanf(sys_line, "%*s %s %i %*s %i", device_name, &read_speed, &write_speed) == 3)
            {
                strcpy(devices[ndevices].device_name, device_name);
                devices[ndevices].read_speed = read_speed;
                devices[ndevices].write_speed = write_speed;
                ndevices++;
                printf("%i %s\t %i\t %i\n", ndevices, device_name, read_speed, write_speed);
            }
        }
        fclose(sysconfig);
    }
}



void read_commands(char argv0[], char filename[]) {
    FILE *commands = fopen(filename, "r");

    if (commands != NULL) {
        char command_line[BUFSIZE];
        command current_command; // Create a temporary command to store data while reading

        while (fgets(command_line, BUFSIZE, commands) != NULL) {
            // Skip blank lines
            if (strlen(command_line) <= 1) {
                continue;
            }

            // Skip comments
            if (command_line[0] == CHAR_COMMENT) {
                continue;
            }

            // Parse command name (1 string)
            if (command_line[0] != ' ') {
                if (sscanf(command_line, "%s", current_command.command_name) == 1) {
                    printf("Command %i %s\n", ncommands, current_command.command_name);
                    current_command.nsyscalls = 0; // Initialize syscall count for this command

                    while (fgets(command_line, BUFSIZE, commands)) {
                        // Check for different types of syscalls
                        if (strlen(command_line) <= 1) {
                            continue; // Skip blank lines
                        }

                        if (command_line[0] == CHAR_COMMENT) {
                            continue; // Skip comments
                        }

                        // Parse syscall line based on different types
                        int run_time;
                        char syscall_name[MAX_COMMAND_NAME];
                        int argument;
                        char device_name[MAX_DEVICE_NAME];
                        char child_command_name[MAX_COMMAND_NAME];

                        if (sscanf(command_line, "%i usecs %s %i usecs", &run_time, syscall_name, &argument) == 3) {
                            // This is a regular syscall (sleep)
                            // Print and store information about the syscall
                            printf("%i-%i %i %s %i usecs\n", ncommands, current_command.nsyscalls, run_time, syscall_name, argument);
                        } else if (sscanf(command_line, "%i usecs %s %iB", &run_time, syscall_name, &argument) == 3) {
                            // This is a syscall with I/O operation (read, write)
                            // Parse the device name as well
                            if (sscanf(command_line, "%*s %*s %*s %s", device_name) == 1) {
                                // Print and store information about the syscall
                                printf("%i-%i %i %s %iB %s\n", ncommands, current_command.nsyscalls, run_time, syscall_name, argument, device_name);
                            }
                        } else if (sscanf(command_line, "%i usecs %s", &run_time, syscall_name) == 2) {
                            // This is a syscall without an argument (exit)
                            // Print information about the syscall
                            printf("%i-%i %i %s\n", ncommands, current_command.nsyscalls, run_time, syscall_name);
                        } else if (sscanf(command_line, "%i usecs %s %s", &run_time, syscall_name, child_command_name) == 3) {
                            // This is a syscall with spawn and child command (spawn)
                            // Print and store information about the syscall
                            printf("%i-%i %i %s %s\n", ncommands, current_command.nsyscalls, run_time, syscall_name, child_command_name);
                        }

                        // Store syscall information in the current_command structure
                        current_command.sys_calls[current_command.nsyscalls].run_time = run_time;
                        strcpy(current_command.sys_calls[current_command.nsyscalls].syscall_name, syscall_name);
                        strcpy(current_command.sys_calls[current_command.nsyscalls].device_name, device_name);
                        current_command.sys_calls[current_command.nsyscalls].argument = argument;
                        strcpy(current_command.sys_calls[current_command.nsyscalls].child_command_name, child_command_name);

                        current_command.nsyscalls++;
                    }

                    // Add the current_command to the command_list and increment the command count
                    if (ncommands < MAX_COMMANDS) {
                        command_list[ncommands] = current_command;
                        ncommands++;
                    } else {
                        printf("Error: Too many commands to fit in command_list\n");
                        break; // Exit the loop if too many commands
                    }
                } else {
                    printf("Error: Failed to parse command name\n");
                }
            }
        }

        fclose(commands);
    } else {
        printf("Error: Unable to open command file\n");
    }
}


//  ----------------------------------------------------------------------
//  FUNCTIONS TO SIMULATE THE SYSTEM'S OPERATION
//  Running -> Exit        clock 0
//  Ready -> Running       clock +5
//  Running -> Blocked     clock +10
//  Running -> Ready       clock +10
//  Blocked -> Ready       clock +10
//  All process -> first acquires data-bus (clock +20)
//  Process that exhaust their time quantum always return to the ready queue
//  When the CPU is idle (no process or the OS is using the CPU), perform the following in this order:
//  unblock any sleeping commands
//  unblock any processes waiting for all their spawned processes (children) to terminate
//  unblock any completed I/O
//  commence any pending I/O
//  commence/resume the next READY process
//  otherwise the CPU remains idle...

void execute_commands(void)
{
    /*
    //  Step 1: Create ready queue, blocked queue, running queue
    init_queue(&ready_queue);
    init_queue(&blocked_queue);
    init_queue(&running_queue);


    //  Step 2: loop through the commands
    //  add commands to ready queue and print out the content of the ready queue
    //  move commands from ready queue to running queue, CLOCK + 5
    //  update the global clock to account for the time taken to move the syscalls from ready to running to the CPU
    //  execute first syscall of the process for run_time, CLOCK + run_time
    //  nsyscalls + 1
    //  update the total time that all syscalls have spent on the CPU, TOTAL_PROCESS_TIME_ON_CPU = argument + TOTAL_PROCESS_TIME_ON_CPU
    //  print out the content of the running queue
    //  print information about the command being executed, including its PID, the command name, and the time it was started
    //  check if the command is a blocking command (sleep or wait), if it is, move it to the blocked queue, block time is the time
    //  if the command is not a blocking command, move it to the ready queue, clock + 10
    //  if the command run_time is over thank time quantum, move it to the ready queue, clock + 10
    //  check next command check if the command is spawn, if it is, create a new process for the child command, add the child command to the ready queue, clock + 10
    //  check next command check if the command is exit, if it is, move the command out of running queue, clock + 0
    //  when nprocesses == 0, exit the loop
    for(int i = 0; i < ncommands; i++)
    {
        add_to_queue(&ready_queue, &command_list[i]);
        printf("Ready Queue: %s\n", ready_queue.queue_name);

        nprocesses++;
        moveFromReadyToRunning(&ready_queue, &running_queue);
        GLOBAL_CLOCK += 5;
        printf("Running Queue: %s\n", running_queue.queue_name);
        printf("Command %i %s started at %i\n", i, command_list[i].command_name, GLOBAL_CLOCK);
        for(int j = 0; j < command_list[i].nsyscalls; j++)
        {
            GLOBAL_CLOCK += command_list[i].syscalls[j].run_time;
            TOTAL_PROCESS_TIME_ON_CPU += command_list[i].syscalls[j].argument;
            printf("Command %i %s %i %s %i\n", i, command_list[i].command_name, j, command_list[i].syscalls[j].syscall_name, GLOBAL_CLOCK);
            if(strcmp(command_list[i].syscalls[j].syscall_name, "sleep") == 0 || strcmp(command_list[i].syscalls[j].syscall_name, "wait") == 0)
            {
                addToBlockedQueue(&blocked_queue, &command_list[i]);
                printf("Blocked Queue: %s\n", blocked_queue.queue_name);
            }
            else
            {
                add_to_queue(&ready_queue, &command_list[i]);
                printf("Ready Queue: %s\n", ready_queue.queue_name);
                GLOBAL_CLOCK += 10;
            }
            if(command_list[i].syscalls[j].run_time > time_quantum)
            {
                add_to_queue(&ready_queue, &command_list[i]);
                printf("Ready Queue: %s\n", ready_queue.queue_name);
                GLOBAL_CLOCK += 10;
            }
            if(strcmp(command_list[i].syscalls[j].syscall_name, "spawn") == 0)
            {
                // create a new process for the child command
                // add the child command to the ready queue
                // clock + 10
                add_to_queue(&ready_queue, &command_list[i]);
                printf("Ready Queue: %s\n", ready_queue.queue_name);
                GLOBAL_CLOCK += 10;
            }

            // move the command out of running queue, clock + 0
            if(strcmp(command_list[i].syscalls[j].syscall_name, "exit") == 0)
            {

            }
        }
    }
*/
}

//  ----------------------------------------------------------------------
// main() function to start the program

int main(int argc, char *argv[])
{
//  ENSURE THAT WE HAVE THE CORRECT NUMBER OF COMMAND-LINE ARGUMENTS
    if(argc != 3) {
        printf("Usage: %s sysconfig-file command-file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

//  READ THE SYSTEM CONFIGURATION FILE
    read_sysconfig(argv[0], argv[1]);

//  READ THE COMMAND FILE
    read_commands(argv[0], argv[2]);

//   init_processes();
//    init_READY_queue();
//    init_BLOCKED_queue();

//  EXECUTE COMMANDS, STARTING AT FIRST IN command-file, UNTIL NONE REMAIN
    execute_commands();

//  PRINT THE PROGRAM'S RESULTS
    printf("measurements  %i  %i\n", GLOBAL_CLOCK, 100*TOTAL_PROCESS_TIME_ON_CPU/GLOBAL_CLOCK);

    exit(EXIT_SUCCESS);
}

//  vim: ts=8 sw=4
