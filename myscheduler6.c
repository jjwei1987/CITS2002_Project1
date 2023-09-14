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
//  STRUCTURES
struct sysconfig{
    char        device_name[MAX_DEVICE_NAME];
    int         read_speed;
    int         write_speed;
} devices[MAX_DEVICES];

struct commands{
    int        when_evoked;
    char       command_name[MAX_COMMAND_NAME];
    int        nsyscalls;
    int        total_execution_time;
    struct
    {
        int         run_time;
        char        syscall_name[MAX_COMMAND_NAME];
        char        device_name_of_syscall[MAX_DEVICE_NAME];
        int         syscall_argument;
        char        child_command_name[MAX_COMMAND_NAME];
    }syscalls[MAX_SYSCALLS_PER_PROCESS];
} command_list[MAX_COMMANDS];

struct queue{
    char queue_name[MAX_COMMAND_NAME];
    int front;
    int end;
}processes[MAX_RUNNING_PROCESSES];

// Define a struct for reexecution items
struct reexecution_item {
    char command_name[MAX_COMMAND_NAME]; // Assuming you want to store the command name
    int remaining_execution_time; // The remaining execution time
};

// ----------------------------------------------------------------------
//  Queue and FUNCTIONS

void init_queue(struct queue *q){
    q->front = -1;
    q->end = -1;
}

int is_queue_empty(struct queue *q)
{
    if (q->front == -1)
    {
        printf("Queue is empty\n");
        return 1;
    }
    return 0;
}

int is_queue_full(struct queue *q)
{
    if (q->end == MAX_RUNNING_PROCESSES - 1 && q->front == 0)
    {
        printf("Queue is full\n");
        return 1;
    }
    return 0;
}

int add_to_queue(struct queue *q, char *command, const char *queue_type)
{
    if (is_queue_full(q))
    {
        printf("Queue is full\n");
        return -1;
    }
    else
    {
        if (q->front == -1)
        {
            q->front = 0;
        }
        q->end = (q->end + 1) % MAX_RUNNING_PROCESSES; // Update end index properly
        strcpy(q->queue_name, command);  // Copy the string

        // Customise the print statement based on the queue type
        if (strcmp(queue_type, "ready") == 0)
        {
            printf("Command %s added to ready queue\n", command);
        }
        if (strcmp(queue_type, "reexecution") == 0)
        {
            printf("Command %s added to reexecution queue\n", command);
        }
        else if (strcmp(queue_type, "blocked") == 0)
        {
            printf("Command %s added to blocked queue\n", command);
        }

        return 0;
    }
}


void move_out_of_queue(struct queue *q, char *command)
{
    // Move a process from ready_queue to running
    if (!is_queue_empty(q))
    {
        q->front = (q->front + 1) % MAX_RUNNING_PROCESSES; // Update front index properly
    }
}
// ----------------------------------------------------------------------
// FUNCTIONS TO READ THE SYSTEM CONFIGURATION FILE AND COMMAND FILE

void read_sysconfig(char argv0[], char filename[])
{
    FILE *sysconfig = fopen(filename, "r");

    if(sysconfig != NULL)
    {
        char sys_line[BUFSIZE];
        while(fgets(sys_line, BUFSIZE, sysconfig) != NULL)
        {
            if(sys_line[0] == CHAR_COMMENT)
            {
                continue;
            }
            
            
            {
                char device_name[MAX_DEVICE_NAME];
                int read_speed;
                int write_speed;
                // read the device name, read speed and write speed, skip the first string, devices array size is 3
                if ((sscanf(sys_line, "%*s %s %i %*s %i", device_name, &read_speed, &write_speed) == 3))
                {
                    strcpy(devices[ndevices].device_name, device_name);
                    devices[ndevices].read_speed = read_speed;
                    devices[ndevices].write_speed = write_speed;
                    ndevices++;
                    printf("%i %s\t %i\t %i\n", ndevices, device_name, read_speed, write_speed);
                }
                
            }

            if(sys_line[0] == 't')
            {
                if(sscanf(sys_line, "%*s %i", &time_quantum) == 1)
                {
                    printf("Time Quantum: %i\n", time_quantum);
                }
            } 
        }
    }
    fclose(sysconfig);

}

void read_commands(char argv0[], char filename[])
{
    FILE *commands = fopen(filename, "r");

    if (commands != NULL)
    {
        char command_line[BUFSIZE];
        while (fgets(command_line, BUFSIZE, commands) != NULL)
        {
        //skip the "#" line
            if (command_line[0] == CHAR_COMMENT)
            {
                continue;
            }
        //read command name, 1 string
            if(command_line[0] != ' ' && command_line[0] != CHAR_COMMENT)
            {
                if(sscanf(command_line, "%s", command_list[ncommands].command_name) == 1)
                {
                    printf("Command %i %s\n", ncommands, command_list[ncommands].command_name);
                
                
                    while (fgets(command_line, BUFSIZE, commands))
                    {
                        
                        //read normal system call (3), 1 integer, usecs, 1 string, 1 integer
                        if(sscanf(command_line, "%i usecs %s %i usecs", 
                                &command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].run_time, 
                                command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].syscall_name, 
                                &command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].syscall_argument) == 3)
                        {
                            printf("%i-%i %i %s %i usecs\n",
                                    ncommands,
                                    command_list[ncommands].nsyscalls, 
                                    command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].run_time, 
                                    command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].syscall_name, 
                                    command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].syscall_argument);
                            command_list[ncommands].nsyscalls++;
                        }                        

                        //read IO system call (4), 1 integer, usecs, 1 string, 1 string, 1 integer
                        else if(sscanf(command_line, "%i usecs %s %s %i B", 
                                &command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].run_time,
                                command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].syscall_name,
                                command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].device_name_of_syscall,
                                &command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].syscall_argument) == 4)
                        {
                            printf("%i-%i %i %s %s %i\n",
                                    ncommands,
                                    command_list[ncommands].nsyscalls,
                                    command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].run_time,
                                    command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].syscall_name,
                                    command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].device_name_of_syscall,
                                    command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].syscall_argument);
                            command_list[ncommands].nsyscalls++;
                        }

                        //read spawn call
                        else if(sscanf(command_line, "%i usecs %s %s", 
                                &command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].run_time, 
                                command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].syscall_name, 
                                command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].child_command_name) == 3)
                        {
                            printf("%i-%i %i %s %s\n",
                                    ncommands,
                                    command_list[ncommands].nsyscalls,
                                    command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].run_time, 
                                    command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].syscall_name, 
                                    command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].child_command_name);
                            command_list[ncommands].nsyscalls++;
                        }

                        //read exit and wait call (2), 1 integer, 1 strings
                        else if(sscanf(command_line, "%i usecs %s", 
                                &command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].run_time, 
                                command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].syscall_name) == 2)
                        {
                            printf("%i-%i %i %s\n",
                                    ncommands, 
                                    command_list[ncommands].nsyscalls,
                                    command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].run_time, 
                                    command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].syscall_name);

                            if(strcmp(command_list[ncommands].syscalls[command_list[ncommands].nsyscalls].syscall_name, "exit") == 0)
                            {
                                break;
                            }
                        }

                        else
                        {
                            printf("Error: Invalid command\n");
                        }
                    }
                    ncommands++;   

                }    
            }
        }
    }
    fclose(commands);
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
//  unblock any sleeping processes
//  unblock any processes waiting for all their spawned processes (children) to terminate
//  unblock any completed I/O
//  commence any pending I/O
//  commence/resume the next READY process
//  otherwise the CPU remains idle...

void execute_commands(void)
{
    // Step 1: Create ready queue, blocked queue, running queue, and reexecution queue
    struct queue ready_queue;
    struct queue blocked_queue;
    struct queue running_queue;
    struct queue reexecution_queue;
    init_queue(&ready_queue);
    init_queue(&blocked_queue);
    init_queue(&running_queue);
    init_queue(&reexecution_queue);


    // Step 2: Loop through the commands
    for (int i = 0; i < ncommands; i++)
    {
        // Add commands to the ready queue and print out the content of the ready queue
        add_to_queue(&ready_queue, command_list[i].command_name, "ready");
        printf("Ready Queue: %s\n", ready_queue.queue_name);
        
        // Move commands from ready queue to running queue, CLOCK + 5
        move_out_of_queue(&ready_queue, command_list[i].command_name);
        GLOBAL_CLOCK += TIME_CONTEXT_SWITCH; // Increment for the move to the running queue
        add_to_queue(&running_queue, command_list[i].command_name, "running");
        // Print information about the command being moved to the running queue
        printf("Command %s ready to running %i\n", command_list[i].command_name, GLOBAL_CLOCK);
        
        
        // Execute the first syscall of the process for run_time, CLOCK + run_time
        int execution_time = command_list[i].syscalls[0].run_time;

        
        // Check if the command run_time is over the time quantum
        if (execution_time > time_quantum)
        {
            // Move it to the ready queue and update the run_time
            add_to_queue(&ready_queue, command_list[i].command_name, "ready");
            command_list[i].syscalls[0].run_time -= time_quantum;
            printf("Ready Queue: %s\n", ready_queue.queue_name);
            GLOBAL_CLOCK += TIME_CORE_STATE_TRANSITIONS;
            printf("Command %s moved back to the ready queue at %i\n", command_list[i].command_name, GLOBAL_CLOCK);
        }
        // Check if the command's current syscall is "exit"
        if (strcmp(command_list[i].syscalls[0].syscall_name, "exit") == 0)
        {
            int remaining_execution_time = command_list[i].syscalls[0].run_time - command_list[i].total_execution_time;
            if (remaining_execution_time > 0)
            {
                TOTAL_PROCESS_TIME_ON_CPU += remaining_execution_time;
                GLOBAL_CLOCK += remaining_execution_time;
                printf("Running Queue: %s\n", running_queue.queue_name);
                // Update the total_execution_time for the current command
                command_list[i].total_execution_time += execution_time;
                printf("Command %s executing %i\n", command_list[i].command_name, GLOBAL_CLOCK);
            }

            // Remove the command from the running queue
            move_out_of_queue(&running_queue, command_list[i].command_name);
        }

        else
        {
            // Update the total_execution_time and TOTAL_PROCESS_TIME_ON_CPU for the current command
            command_list[i].total_execution_time += execution_time;
            TOTAL_PROCESS_TIME_ON_CPU += execution_time;
            printf("Running Queue: %s\n", ready_queue.queue_name);
            
            GLOBAL_CLOCK += execution_time;
            // Print information about the command being executed
            printf("Command %s executing %i\n", command_list[i].command_name, GLOBAL_CLOCK);
        }
        
        // Check if the command's current syscall is "sleep" or "wait"
        if (strcmp(command_list[i].syscalls[0].syscall_name, "sleep") == 0 || strcmp(command_list[i].syscalls[0].syscall_name, "wait") == 0)
        {
            // Move command to the blocked queue
            add_to_queue(&blocked_queue, command_list[i].command_name, "blocked");
            GLOBAL_CLOCK += TIME_CORE_STATE_TRANSITIONS; // Increment for the move to blocked queue
            // Print information about the command being moved to the blocked queue
            printf("Command %s running to blocked %i\n", command_list[i].command_name, GLOBAL_CLOCK);
            printf("Blocked Queue: %s\n", ready_queue.queue_name);
            
            if (strcmp(command_list[i].syscalls[0].syscall_name, "sleep") == 0)
            {
                int sleep_time = command_list[i].syscalls[0].syscall_argument;
                if (sleep_time > 0)
                {
                    GLOBAL_CLOCK += sleep_time;
                    // Print information about the command being moved to the running queue
                    printf("Command %s in sleep state %i\n", command_list[i].command_name, GLOBAL_CLOCK);
                    
                    // Set the sleep syscall's duration to 0
                    command_list[i].syscalls[0].run_time = 0;
                    // Increment ncommands and add the command to the end of the command_list
                    ncommands++;
                    strcpy(command_list[ncommands - 1].command_name, command_list[i].command_name);
                    command_list[ncommands - 1].nsyscalls = 0;
                    // Print information about the command completing the sleep cycle
                    printf("%s: completed sleep cycle \n", command_list[i].command_name);
                    GLOBAL_CLOCK += TIME_CORE_STATE_TRANSITIONS;
                    // Print information about the command being moved to the running queue
                    printf("Command %s blocked to ready %i\n", command_list[i].command_name, GLOBAL_CLOCK);
                    move_out_of_queue(&blocked_queue, command_list[i].command_name);
                    
                }
                
                else if (strcmp(command_list[i].syscalls[0].syscall_name, "wait") == 0)
                {
                    // Handle wait syscall
                }
            }
        }
        
        
        else if (strcmp(command_list[i].syscalls[0].syscall_name, "spawn") == 0)
        {
            add_to_queue(&ready_queue, command_list[i].syscalls[0].child_command_name, "ready");
            printf("Ready Queue: %s\n", ready_queue.queue_name);
            GLOBAL_CLOCK += TIME_CORE_STATE_TRANSITIONS;
        }
        /*
        else if (strcmp(command_list[i].syscalls[0].syscall_name, "exit") == 0)
        {
            
            move_out_of_queue(&running_queue, command_list[i].command_name);
            printf("Running Queue: %s\n", running_queue.queue_name);
        }
        if (nprocesses == 0)
        {
            break;
        }*/
    }
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
