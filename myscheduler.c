#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


//  CITS2002 Project 1 2023
//  Student1:   STUDENT-NUMBER1   NAME-1
//  Student2:   STUDENT-NUMBER2   NAME-2


//  myscheduler (v1.0)
//  Compile with:  cc -std=c11 -Wall -Werror -o myscheduler myscheduler.c


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
    struct{
        int         run_time;
        char        syscall_name;
        char        device_name_of_syscall;
        int         syscall_argument;
    }syscalls[MAX_SYSCALLS_PER_PROCESS];
} commands[MAX_COMMANDS];

struct queue{
    char queue_name[MAX_COMMAND_NAME];
    int front;
    int end;
}processes[MAX_RUNNING_PROCESSES];
// ----------------------------------------------------------------------
//  FUNCTIONS

void init_queue(struct queue *queue){
    queue->front = -1;
    queue->end = -1;
}

void is_queue_empty(struct queue *queue)
{
    if(queue->processes[0].process_id == 0)
    {
        printf("Queue is empty\n");
    }
    else
    {
        printf("Queue is not empty\n");
    }
}

void is_queue_full(struct queue *queue)
{
    if(queue->processes[MAX_RUNNING_PROCESSES].process_id != 0)
    {
        printf("Queue is full\n");
    }
    else
    {
        printf("Queue is not full\n");
    }
}

int enqueue(){

}

int dequeue(){

}

void read_sysconfig(char argv0[], char filename[])
{
    FILE *sysconfig = fopen(filename, "r");

    if(sysconfig != NULL)
    {
        char line[BUFSIZE];
        while(fgets(line, sizeof(line), sysconfig) != NULL)
        {
            if(line[0] == CHAR_COMMENT)
            {
                continue;
            }
            else
            {
                char device_name[MAX_DEVICE_NAME];
                int read_speed;
                int write_speed;
                sscanf(line, "%s %i %i", device_name, &read_speed, &write_speed);
                strcpy(devices[ndevices].device_name, device_name);
                devices[ndevices].read_speed = read_speed;
                devices[ndevices].write_speed = write_speed;
                ndevices++;
            }
        }
    }
    else
    {
        printf("Error: Cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

}

void read_commands(char argv0[], char filename[])
{
}

//  ----------------------------------------------------------------------
//  Ready -> Running       clock +5
//  Running -> Blocked     clock +10
//  Running -> Ready       clock +10
//  Blocked -> Ready       clock +10
//  All process -> first acquires data-bus (clock +20)
void execute_commands(void)
{
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

//    verbose = verbose || (getenv("HTTPS") != NULL);

//  READ THE SYSTEM CONFIGURATION FILE
    read_sysconfig(argv[0], argv[1]);
    dump_sysconfig();
    exit(0);

//  READ THE COMMAND FILE
    read_commands(argv[0], argv[2]);
    dump_commands();
    exit(0);

    init_processes();
    init_READY_queue();
    init_BLOCKED_queue();

//  EXECUTE COMMANDS, STARTING AT FIRST IN command-file, UNTIL NONE REMAIN
    execute_commands();

//  PRINT THE PROGRAM'S RESULTS
    printf("measurements  %i  %i\n", GLOBAL_CLOCK, 100*TOTAL_PROCESS_TIME_ON_CPU/GLOBAL_CLOCK);

    exit(EXIT_SUCCESS);
}

//  vim: ts=8 sw=4