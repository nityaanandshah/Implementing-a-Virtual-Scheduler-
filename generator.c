#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    // Initializing terminal emulator file path
    char xterm_PATH[] = "/usr/bin/xterm";
    char sys_PATH[] = "Desktop/IITJ/SEM-05/OperatingSystems/Lab04/process.out ";

    // Considering 0 for cpu bound and 1 for the io bound;
    int pid;
    int n = 2;
    int *NOP = (int *)malloc(n * sizeof(int));
    int *priority = (int *)malloc(n * sizeof(int));
    int *inactiveTime = (int *)malloc(n * sizeof(int));
    int *sleep_prob = (int *)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++)
    {
        NOP[i] = i == 0 ? 10000 : 4000;
        priority[i] = i == 0 ? 10 : 5;
        inactiveTime[i] = i == 0 ? 1 : 3;
        sleep_prob[i] = i == 0 ? 30 : 70;
    }

    // 4 processes in 1 sec execute
    int i, j = 0;
    char call[10001], add[10001];

    for (; i < 2 * n; i++)
    {
        // NOP, priority, sleepsleep_prob, time
        memset(&call, 0, 10001);
        sprintf(call, sys_PATH);
        sprintf(add, "%d ", NOP[j]);
        strcat(call, add);
        sprintf(add, "%d ", priority[j]);
        strcat(call, add);
        sprintf(add, "%d ", sleep_prob[j]);
        strcat(call, add);
        sprintf(add, "%d ", inactiveTime[j]);
        strcat(call, add);
        printf("checking value for the call =%s \n", call);

        // Forking the xterm for every process
        pid = fork();
        if (pid == 0)
            execl(xterm_PATH, xterm_PATH, "-e", "bash", "-c", call, (void *)NULL);
        if (i >= 1)
            j = 1;
    }

    free(NOP);
    free(priority);
    free(inactiveTime);
    free(sleep_prob);

    return 0;
}