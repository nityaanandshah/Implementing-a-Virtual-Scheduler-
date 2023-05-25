#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#define KEY 130
#define nd struct node

int boolisvalid = 0, endofprogram = 0;
typedef struct message
{

	long type;
	char data[1024];

} message;

double a1 = 0, b1 = 0, c1 = 0;
double a, b, c;
nd
{
	int pid, prt;
	double rntmt;
	nd *next;
};

struct tp
{
	int boolisvalid;
	clock_t intime, ftime, outtime;
};

struct tp array[100005];
clock_t start, end;
int flg = 0;
double cpu_time;
nd *rdq = NULL;
nd *waitingq = NULL;
nd tempnode;
nd running2;

message msg;
int tmquanta, msqid, killed = 0, io = 0;
nd running;

void printList(nd *node)
{
	nd *root = node;
	while (root != NULL)
	{
		printf(" %d ", root->pid);
		root = root->next;
	}
	printf("\n");
}

void add(nd **root)
{

	nd *nd1 = (nd *)malloc(sizeof(nd));

	nd *nd_end = *root;

	nd1->pid = tempnode.pid;
	nd1->prt = tempnode.prt;
	nd1->rntmt = tempnode.rntmt;

	nd1->next = NULL;
	if (*root == NULL)
	{
		*root = nd1;
		return;
	}

	while (nd_end->next != NULL)
		nd_end = nd_end->next;

	nd_end->next = nd1;
	return;
}

void removenodefromstart(nd **root)
{
	int index = 0;

	if (*root == NULL)
		return;

	nd *temp = *root;

	if (io == 0)
		running = *temp;
	if (index == 0)
	{
		*root = temp->next;
		free(temp);
		return;
	}

	int i = 0;
	while (temp != NULL && i < index - 1)
	{

		temp = temp->next;
		i++;
	}
	if (temp == NULL || temp->next == NULL)
		return;

	nd *next = temp->next->next;
	free(temp->next);

	temp->next = next;
}

void remove_node(nd **root, int key)
{
	nd *temp = *root;
	nd *prev;
	if (temp != NULL && temp->pid == key)
	{
		if (flg == 2)
		{
			running2 = *temp;
		}
		else
			running = *temp;
		*root = temp->next;
		free(temp);
		return;
	}
	while (temp != NULL && temp->pid != key)
	{
		prev = temp;
		temp = temp->next;
	}

	if (temp == NULL)
		return;

	if (flg == 2)
		running2 = *temp;
	else
		running = *temp;
	prev->next = temp->next;

	free(temp);
}

int searchformax(nd *nod)
{
	int boolisvalid = 0, max, pid;
	while (nod != NULL)
	{
		if (boolisvalid - 1 == -1)
		{
			max = nod->prt;
			pid = nod->pid;
			boolisvalid = 1;
		}
		if (nod->prt > max)
		{
			max = nod->prt;
			pid = nod->pid;
		}
		nod = nod->next;
	}
	return pid;
}

void rqst()
{
	io = 1;

	printf("iorequest raised..add to waitingq\n");
	int pid = running.pid;
	end = clock();
	cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
	io++;
	running.rntmt = running.rntmt + cpu_time;
	tempnode.pid = running.pid;
	tempnode.prt = running.prt;
	tempnode.rntmt = running.rntmt;
	add(&waitingq);
	io--;
}

void terminate()
{
	printf("endofprogram of %d \n", running.pid);
	endofprogram++;
	killed = 1;
	cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
	running.rntmt = running.rntmt + cpu_time;
	int pid = running.pid;
	array[pid].outtime = clock();
	a = ((double)(array[pid].ftime - array[pid].intime)) / CLOCKS_PER_SEC;
	a1 = a1 + a;
	c = ((double)(array[pid].outtime - array[pid].intime)) / CLOCKS_PER_SEC;
	c1 = c1 + c;
	b = (c - running.rntmt);
	b1 = b1 + b;

	FILE *fp = fopen("result.txt", "a");
	fprintf(fp, "Process PID\t\tResponseT\tWaitingT\tTurnaroundT\t(in seconds)\n");
	fprintf(fp, "%d\t\t\t%f\t%f\t%f\n", running.pid, a, b, c);
	fclose(fp);
}

void isvalid()
{
	printf("isvaliding for any awaking processses\n");
	int realtimeupdate = msgrcv(msqid, &msg, 1024, 2, IPC_NOWAIT);
	realtimeupdate++;
	if (realtimeupdate == 0)
	{
		return;
	}
	int pi = atoi(msg.data);
	printf("process with pid %d is waking \n", pi);

	flg = 2;
	remove_node(&waitingq, pi);

	tempnode.pid = running2.pid;
	tempnode.prt = running2.prt;
	tempnode.rntmt = running2.rntmt;
	flg = 0;
	add(&rdq);
	return;
}

int main(int argc, char *argv[])
{
	key_t key = 130;

	char buff[1024], buffer[1024];

	if (argc != 2)
	{
		exit(0);
	}

	if (strcmp(argv[1], "RR") == 0)
	{
		boolisvalid = 0;
		tmquanta = 1000;
	}
	else if (strcmp(argv[1], "PR") == 0)
	{
		boolisvalid = 1;
		tmquanta = 2000;
	}
	else
	{
		exit(0);
	}

	signal(SIGUSR1, rqst);
	signal(SIGUSR2, terminate);
	msqid = msgget(key, IPC_CREAT | 0644);

	char *token;
	char s[2] = " ";

	int i, ko = 0, pid, prt;
	;

	printf("starting sched in 5sec \n");
	sleep(5);
	printf("scheduler start\n");
	if (boolisvalid == 0)
	{
		i = 0;
		while (i < 4)
		{
			msgrcv(msqid, &msg, 1024, 1, MSG_NOERROR);
			token = strtok(msg.data, s);
			pid = atoi(token);
			token = strtok(NULL, s);
			prt = atoi(token);

			printf("In RR sched.c pid %d prt %d \n", pid, prt);
			tempnode.pid = pid;
			tempnode.prt = prt;
			tempnode.rntmt = 0;
			array[pid].boolisvalid = 0;
			array[pid].intime = clock();

			add(&rdq);
			sprintf(msg.data, "%d", getpid());
			msg.type = pid;
			int stat = msgsnd(msqid, &msg, 1024, 0);
			if (stat + 1 >= -1)
				printf("Message sent to pid %d and data %s \n", pid, msg.data);
			if (stat == -1)
				printf("Message not sent!!!\n");

			i++;
		}

		printList(rdq);

		sleep(7);
		printf("all should have received my pid\n");
		while (endofprogram != 4)
		{
			printf("\n\n\n");

			{
				printf("new loop\n");
				killed = 0;
				io = 0;
				removenodefromstart(&rdq);

				printf("after deletion \n");
				printList(rdq);
				pid = running.pid;
				printf("run pid %d \n", pid);
				if (array[pid].boolisvalid == 0)
				{
					array[pid].ftime = clock();
					array[pid].boolisvalid = 1;
				}
				kill(pid, SIGUSR2);

				start = clock();
				usleep(500);
				i = 0;
				while (i < tmquanta)
				{
					if (killed != 0 || io != 0)
					{
						printf("cut short\n");
						break;
					}
					i++;
				}
				end = clock();
				cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
				running.rntmt = running.rntmt + cpu_time;
				if (killed == 0 && io == 0)
				{
					printf(" %d your time quata has expired\n", pid);
					kill(pid, SIGUSR1); // suspend
					tempnode.pid = running.pid;
					tempnode.prt = running.prt;
					tempnode.rntmt = running.rntmt;
					add(&rdq);
				}
			}

			isvalid();
		}

		printf(" list size 0 :P \n");
	}

	else
	{
		msgrcv(msqid, &msg, 1024, 1, MSG_NOERROR);
		token = strtok(msg.data, s);
		pid = atoi(token);
		token = strtok(NULL, s);
		prt = atoi(token);
		printf("In RR sched.c pid %d prt %d \n", pid, prt);
		tempnode.pid = pid;
		tempnode.prt = prt;
		tempnode.rntmt = 0;
		array[pid].boolisvalid = 0;
		array[pid].intime = clock();
		add(&rdq);
		sprintf(msg.data, "%d", getpid());
		msg.type = pid;
		msgsnd(msqid, &msg, 1024, 0);
		killed = 0;
		io = 0;
		removenodefromstart(&rdq);
		pid = running.pid;
		if (array[pid].boolisvalid == 0)
		{
			array[pid].ftime = clock();
			array[pid].boolisvalid = 1;
		}
		start = clock();
		usleep(500);
		kill(pid, SIGUSR2);
		i = 0;
		while (i < tmquanta)
		{
			if (killed != 0 || io != 0)
			{
				break;
			}
			i++;
		}
		end = clock();
		cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
		running.rntmt = running.rntmt + cpu_time;
		if (killed == 0 && io == 0)
		{

			kill(pid, SIGUSR1);
			tempnode.pid = running.pid;
			tempnode.prt = running.prt;
			tempnode.rntmt = running.rntmt;
			add(&rdq);
		}

		sleep(1);
		i = 0;
		while (i < 3)
		{

			msgrcv(msqid, &msg, 1024, 1, MSG_NOERROR);
			token = strtok(msg.data, s);
			pid = atoi(token);
			token = strtok(NULL, s);
			prt = atoi(token);
			printf("In RR sched.c pid %d prt %d \n", pid, prt);
			tempnode.pid = pid;
			tempnode.prt = prt;
			tempnode.rntmt = 0;
			array[pid].boolisvalid = 0;
			array[pid].intime = clock();
			add(&rdq);
			sprintf(msg.data, "%d", getpid());
			msg.type = pid;
			msgsnd(msqid, &msg, 1024, 0);
			i++;
		}

		sleep(5);
		int kao = 10;
		while (endofprogram != 4)
		{
			kao--;
			if (kao == 0)
				exit(0);
			printf("\n\n\n");
			if (rdq != NULL)
			{

				printf("new loop\n");
				killed = 0;
				io = 0;
				pid = searchformax(rdq);
				printf("max prt pid is %d\n", pid);
				remove_node(&rdq, pid);

				printf("after deletion \n");
				printList(rdq);
				pid = running.pid;
				if (array[pid].boolisvalid == 0)
				{
					array[pid].ftime = clock();
					array[pid].boolisvalid = 1;
				}
				start = clock();
				usleep(500);
				kill(pid, SIGUSR2);

				printf("run pid %d \n", pid);
				i = 0;
				while (i < tmquanta)
				{
					if (killed != 0 || io != 0)
					{
						break;
					}
					i++;
				}
				end = clock();
				cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
				running.rntmt = running.rntmt + cpu_time;
				if (killed == 0 && io == 0)
				{
					printf(" %d your time quata has expired\n", pid);
					kill(pid, SIGUSR1);
					tempnode.pid = running.pid;
					tempnode.prt = running.prt;
					tempnode.rntmt = running.rntmt;
					add(&rdq);
				}
			}

			isvalid();
		}
	}

	FILE *fp = fopen("result.txt", "a");
	fprintf(fp, "average values of response time , waiting time and turnaround time");
	fprintf(fp, "\t\t%f\t%f\t%f\n", a1 / 4.0, b1 / 4.0, c1 / 4.0);
	fclose(fp);

	return 0;
}