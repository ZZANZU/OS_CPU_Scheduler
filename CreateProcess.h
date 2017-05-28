/* 
* 2017-05-22 ChanJoo 
*
* OS Term Project
* CPU Scheduling Simulator
*
*
*/

#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define PROCESS_NUM_MAX 11; // the number of processes should be under 15.
#define BURST_MAX 10; // the maximum burst time.
#define QUEUE_MAX 20;
#define ARRIVAL_MAX 7;

typedef struct Process{
	int PID;
	int ArrivalTime;
	int BurstTimeCPU;
	int BurstTimeIO;
	int Priority;
}Process;

typedef struct Node{
	Process* data;
	struct Node *next;
}Node;

typedef struct Queue{
	Node *front;
	Node *rear;
	int count; // num of process in queue.
}Queue;

void InitQueue(Queue *queue);
int IsQueueEmpty(Queue *queue);
void Enqueue(Queue *queue, Process *process);
Process *Dequeue(Queue *queue);
Process *CreateProcess();
void ShowProcess(Queue *queue, Process *process);

Queue* CopyQueue(Queue *copiedQueue, Queue *readyQueue);

/*---------------------------------------------------*/

void InitQueue(Queue *queue){
	queue->front = queue->rear = NULL;
	queue->count = 0;
}

Process* CreateProcess(Queue *queue){
	srand((unsigned int)time(NULL));
	int i;
	
	Process *process;
	
	process = (Process*)malloc(sizeof(Process));
	
	int numOfProcess = 4 + rand() % PROCESS_NUM_MAX;

	// Assigning data to the processes
	for(i = 0 ; i < numOfProcess ; i++){
		process[i].PID = 1 + random() % 98;
		process[i].ArrivalTime = 1 + random() % ARRIVAL_MAX; // hmm...
		process[i].BurstTimeCPU = 3 + random() % BURST_MAX;
		process[i].BurstTimeIO = 3 + random() % BURST_MAX;
		process[i].Priority = 1 + random() % numOfProcess;
		
		Enqueue(queue, &process[i]);
	}
	
	return process;
}

void ShowProcess(Queue *queue, Process *process){
	printf(" process[  ]   PID    Arrival Time   CPU Burst Time   I/O Burst Time   Priority \n");
	printf("--------------------------------------------------------------------------------\n");
	
	for(int i = 0 ; i < queue->count ; i++){
		printf(" process[%2d]   %2d        %2d\t         %2d\t           %2d\t          %2d\n"
		, i
		, process[i].PID
		, process[i].ArrivalTime
		, process[i].BurstTimeCPU
		, process[i].BurstTimeIO
		, process[i].Priority);
		/*
		printf("Process[%d].PID : %d\n", i, process[i].PID);
		printf("Process[%d].ArrivalTime : %d\n", i, process[i].ArrivalTime);
		printf("Process[%d].BurstTimeCPU : %d\n", i, process[i].BurstTimeCPU);
		printf("Process[%d].BurstTimeIO : %d\n", i, process[i].BurstTimeIO);
		printf("Process[%d].Priority : %d\n", i, process[i].Priority);
		*/
	}
	
	printf("--------------------------------------------------------------------------------\n");
}

int IsQueueEmpty(Queue *queue){ return queue->count == 0; }

void Enqueue(Queue *queue, Process *process){
	Node * now = (Node*)malloc(sizeof(Process));
	now->data = process;
	now->next = NULL;
	
	if(IsQueueEmpty(queue)){
		queue->front = now;
	}else{
		queue->rear->next = now;
	}
	
	queue->rear = now;
	queue->count++;
}

Process* Dequeue(Queue *queue){
	Process *returnNode;
	Node *now = (Node*)malloc(sizeof(Process));//
	
	if(IsQueueEmpty(queue)){
		return returnNode;
	}
	
	now = queue->front;
	returnNode = now->data;
	queue->front = now->next;
	
	queue->count--;
	
	free(now);//
	return returnNode;
}

/*-------------------------------------------------------
// from ready queue to copied queue
Queue* CopyQueue(Queue *copiedQueue, Queue *readyQueue){
	int i = 0;
	while(!IsQueueEmpty(readyQueue)){
		Enqueue(copiedQueue, readyQueue->front->data);
		Dequeue(readyQueue);
		printf("enqueue & dequeue : %d\n", i);//test
		i++;//test
	}
	printf("copy completed!\n");//test
	
	return copiedQueue;
}
-------------------------------------------------------*/
