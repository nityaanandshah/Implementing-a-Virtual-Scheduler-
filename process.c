#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/msg.h>

#define MAX 10001

// Using signals for the IPC
// for suspend we are using SIGUSR1 and for notify we are using SIGUSR2

typedef struct message
{
    int messg_type;
    char data[MAX];

} message;

int counter = 0, iterations = 0, flag = 0, i = 0, NOP, priority, sleeptime, msqid, Sleep_prob;
// Pid of the scheduler
int root_pid;      
pid_t proc_pid;

message msg;
int flg = 0;

// Notify signal comes
void notify();

void suspend();

void suspend()
{
    signal(SIGUSR2, notify);
    printf("SUSPEND\n");

    counter = i;
    flg = 1;
}

void notify()
{
    printf("NOTIFY \n");
    flg = 0;
    signal(SIGUSR1, suspend);
}

void call_pause()
{
    // As flag will be 0 so we have to do nothing
    pause();
}

void print_()
{
    printf("No. of process : %d ", NOP);
    printf("Priority = %d", priority);
    printf("Sleep_prob = %d", Sleep_prob);
    printf("SleepTime = %d\n", sleeptime);
}

void kill_process()
{
    // Termination of the process
    // SIGUSSR2-> TERMINATION
    kill(root_pid, SIGUSR2);       
}

void do_fn(int stat)
{
    kill(root_pid, SIGUSR1);

    printf("PID: %d Going for IO\n", proc_pid);
    sleep(sleeptime);

    sprintf(msg.data, "%d", proc_pid);
    msg.messg_type = 2;
    stat = msgsnd(msqid, &msg, MAX, 0);

    // Pause for a notify signal from sheduler
    pause();
    printf("PID: %d returned from IO\n", proc_pid);
}

void iterate()
{
    int pb, stat;
    for (i = counter; i < iterations; i++, counter++)
    {

        while (flg == 1)
        {
            sleep(1);
        }

        pb = 1 + rand() % 100;
        printf("the value of pb is %d\n", pb);
        if (pb <= Sleep_prob)
        {
            do_fn(stat);
        }
    }

    if (i == iterations)
    {
        kill_process();
    }
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    key_t key = 130;

    int pid;
    // To store the string
    char buff[MAX]; 
    char buff2[MAX];
    proc_pid = getpid();

    printf("PID %d\n", proc_pid);

    // Notify and Suspend
    signal(SIGUSR1, suspend);
    signal(SIGUSR2, notify);

    if (argc != 5)
    {
        printf("The Parameters passed are less than expected\n");
        exit(0);
    }

    NOP = atoi(argv[1]);
    priority = atoi(argv[2]);
    Sleep_prob = atoi(argv[3]);
    sleeptime = atoi(argv[4]);

    print_();
    iterations = NOP;

    msqid = msgget(key, 0644 | IPC_CREAT);

    sprintf(buff2, "%d ", proc_pid);
    sprintf(buff, "%d", priority);
    strcat(buff2, buff);
    strcat(msg.data, buff2);
    printf("sending message %s\n", msg.data);

    // messg_type 1 in message queue will be for the NEW process added
    msg.messg_type = 1; 
    int stat = msgsnd(msqid, &msg, MAX, 0);
    if (stat < 0)
        printf("failed\n");

    printf("Waiting for scheduler pid\n");

    // Get the pid of scheduler.c
    // WAIT untill recv message
    msgrcv(msqid, &msg, MAX, proc_pid, MSG_NOERROR); 

    root_pid = atoi(msg.data);
    printf("Scheduler pid : %d \n", root_pid);

    // Scheduler notified waiting for the signal
    printf("waiting for notify\n");
    call_pause();
    flag = 1;
    iterate();

    return 0;
}
