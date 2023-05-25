# Implementing a Virtual Scheduler

## Program Description

The virtual scheduler is implemented on top of the existing Linux scheduler. It chooses one process from the "Ready Queue" based on the policy imposed by the scheduling algorithm. The selected process receives a "notify" signal to (re)start execution, while all other processes in the Ready Queue receive a "suspend" signal to go back into the waiting state until the next scheduling round. The scheduler terminates after all processes have terminated and the Ready Queue is empty.

## Program Components

1. When a process is created in `process.c`, its process ID and priority information are sent to the scheduler.

2. The scheduler adds the process to the end of the "ready queue," which places the process on the run queue.

3. The process stops using `pause()` and waits in the ready queue. Once the scheduler schedules the process, it sends a notification signal, and the process resumes execution.

4. If the process receives a "suspend" signal from the scheduler at any point, it stops using `pause()` and waits for a notification signal.

5. During execution, a process may request I/O, which is simulated using `sleep()`.

6. At each iteration, the process may request I/O based on the "sleep probability" and the duration of the "latency" service.

7. The process informs the scheduler before and after the I/O request.

8. If a process requests I/O, the scheduler removes it from the "ready queue" before `sleep()` and reinserts it at the back of the "ready queue" after `sleep()`.

9. When a process completes its rounds and exits the loop, it reports to the scheduler, and the scheduler removes it from the "ready queue".

10. `generator.c` is used to generate N processes at a specified interval (T). It randomly classifies the processes as CPU-bound or I/O-bound.

## Terminologies Used

- Round: The number of iterations a process will run, signifying the computation duration of a process.
- Priority (1-10): The lower the number, the higher the priority.
- Sleeptime Probability: Specifies the probability of I/O request (sleep) in each iteration.
- Sleeptime: Indicates the duration of the I/O service. All I/O requests for a specific process have the same duration.

## Scheduling Algorithms

Two scheduling algorithms are implemented in `scheduler.c`:

1. RR (Regular Round-Robin): Schedules processes based on their time of arrival without considering priority.
2. PRR (Priority Round-Robin): Schedules processes based on priority and assigns a specific time slot to the highest priority process.

The scheduler selects one process from the ready queue to run and sends a "notify" signal to the scheduled process while sending a "suspend" signal to all other processes.

## Difference between RR and PRR

RR scheduling algorithms schedule processes for a specified time period based on arrival time without considering priority. In the case of priority-based RR scheduling (PRR), the scheduler takes into account the priority of processes and assigns specific time slots based on priority.

## Running the Program

1. Download the provided zip folder and extract all the files.
2. Open the "Lab04" folder and open the terminal in that folder.
3. Run the following commands:

   For generating:
$ gcc generator.c -o generator
$ ./generator

   
   For scheduler:
$ gcc scheduler.c -o scheduler
$ ./scheduler RR
$ ./scheduler PRR

   
   For processes:
$ gcc process.c -o process
$ ./process 1000 10 30 1
