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
#define BURST_MAX 8; // the maximum burst time.
#define QUEUE_MAX 20;
#define ARRIVAL_MAX 7;
#define PRIORITY_MAX 50;

/*---------------------------------------------------*/

typedef struct Process{
	int PID;
	int ArrivalTime;
	int BurstTimeCPU;
	int BurstTimeIO;
	short int Priority;
	char WaitingTime; // 이렇게해야 priority가 튀는 값(딱 하나)이 없어짐. 
	char TurnAroundTime;
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



int numOfProcess; // 랜덤하게 생성된 프로세스들의 개수를 저장. 



/*---------------------------------------------------*/
// Queue 
void InitQueue(Queue *queue);
int IsQueueEmpty(Queue *queue);
void Enqueue(Queue *queue, Process *process);
Process *Dequeue(Queue *queue);

// Process
Process *CreateProcess(Queue* queue);
void ShowProcess(Process *process);
void CopyProcess(Process *process);
void CopyProcess2Queue(Queue *queue, Process *process);

// User Interface
void PrintMenu(Queue *queue);

// scheduling algorithms
void sort_Arrival_Time(Process *process); // #9 Arrival Time에 대해서 sort 
void sort_Priority(Process *process); // #8 Priority에 대해서 sort 
void sort_BurstTime(Process *process); // #15 Burst time에 대해서 sort

void Run_FCFS(Process *process); // #12
void Run_P_Priority(Process *process); // #14
void Run_NP_Priority(Process *process); // #13

int CheckOtherPTime(Process *process, int current_index, int time); // #14-1

/*---------------------------------------------------*/

Process *process_FCFS, 
	 	*process_NP_SJF,
	 	*process_P_SJF,
	 	*process_NP_Priority,
	 	*process_P_Priority,
	 	*process_RR;
	 	
// 각 알고리즘에 해당하는 레디 큐들 
Queue queue_FCFS,
	  queue_NP_SJF,
	  queue_P_SJF,
	  queue_NP_Priority,
	  queue_P_Priority,
	  queue_RR;
	 	
/*---------------------------------------------------*/


int main(){
	int i;
	Queue jobQueue;// ,copiedQueue;
	
	InitQueue(&jobQueue);
	//InitQueue(&copiedQueue);
	
	Process *process;
	process = CreateProcess(&jobQueue);
	
	numOfProcess = jobQueue.count; // 생성된 프로세스의 개수 값 저장 
	
	ShowProcess(process);
	CopyProcess(process); // 랜덤하게 생성한 프로세스를 각 알고리즘에 해당하는 프로세스에 복사
	
	PrintMenu(&jobQueue); // 반드시 process가 각 알고리즘의 process에 copy 된 후에 실행 되어야!(삽질 ㅅㅂ) 
		
}

/*---------------------------------------------------*/

/*
* #17
*
* Round Robin Scheduling 
*
*
*/




/*
* #16
*
* Non-Preemptive Shortest Job Fisrt Scheduling 
*
*
*/
void Run_NP_SJF(Process *process){
	int j, i, time = 0;
	int stop = 0; 
	int gant_chart[100] = {0, };//
	
	Queue terminateQueue;
	
	InitQueue(&terminateQueue);
	
	printf("\n");
	
	// save #1
	
	// burst time 끝난 프로세스를 terminate queue 에 집어넣고, 
	// terminate queue 에 있는 프로세스의 개수가 
	// 처음의 ready queue의 개수와 같아지면 종료
	while(terminateQueue.count != numOfProcess){
		stop = 0;
		
		for(int i = 0 ; i < numOfProcess ; i++){
			
			if(process[i].ArrivalTime <= time && process[i].BurstTimeCPU != 0){
				
				while(process[i].BurstTimeCPU != 0){
					/*waiting time 구하기*/
					if(process[i].WaitingTime == 0){
						process[i].WaitingTime = time - process[i].ArrivalTime;
					}
					
					process[i].BurstTimeCPU--;
					gant_chart[time] = process[i].PID;//
					time++;
				
				}
				
				// Turnaround time
				process[i].TurnAroundTime = time - process[i].ArrivalTime;

				// process[i]의 CPU burst time이 0이 되었으므로 terminate queue로 집어넣음. 
				Enqueue(&terminateQueue, &process[i]); // terminateQueue->count++ 
				stop = 1; // terminate queue에 추가되었으니 line 162의 increment가 일어나면 안됨. 
				
				printf("process[%d] has finished!!! \n",i); // for debugging
				printf("count of terminate queue : %d \n", terminateQueue.count); // for debugging
				
				i = numOfProcess; // for문을 벗어나서 다시 i = 0 부터 시작하기 위해서. line 154로 이동 
			}
		}
		
		if(stop == 0){
			gant_chart[time] = 71;//아무 프로세스도 실행 안됨을 의미
			time++; // enqueue이후에는 이부분이 실행되면 안됨. 
			printf("162 : time = %d\n", time); // for debugging
		}
	} 	
	
	printf("\n");
	
	printf("Non-Preemptive Shortest Job First scheduling finished!\n\n");
	
	ShowProcess(process);
	
	printf("total running time : %d \n", time); 
	
	// 간트차트 출력
	i = 0;
	while(gant_chart[i] != '\0'){
		printf("%2d|",gant_chart[i]);
		i++;
	}
	printf("\n");
	i = 0;
	// 시간 x축 출력 
	while(gant_chart[i] != '\0'){
		printf("%2d|",i+1);
		i++;
	}
	// Evaluation
	float awt = 0; // 함수로 바뀔 수 있으니 여기서 선언
	float tat = 0;
	for(i = 0 ; i < numOfProcess ; i++){
		awt += process[i].WaitingTime;
		tat += process[i].TurnAroundTime;
	}
	printf("\nAverage Waiting time : %f\n", awt/numOfProcess);
	printf("Average Turnaround time : %f\n", tat/numOfProcess);
}

/*
* #15 
*
* Burst Time을 기준으로 sort 
*
*/
void sort_BurstTime(Process *process){
	Process temp_process;
	int i, j;
	
	for(i = 0 ; i < numOfProcess ; i++){
		for(j = 0 ; j < numOfProcess - 1 ; j++){
			if(process[j].BurstTimeCPU > process[j+1].BurstTimeCPU){ // Priority 기준 정렬(내림차순)
				temp_process = process[j];
				process[j] = process[j+1];
				process[j+1] = temp_process;
			}
		}
	}
	
	// arrival time으로도 sorting
	// burst time 같은게 3개면 ㅠㅠ?
	
	
	printf("\nGiven processes were sorted with Burst Time!\n\n");
	ShowProcess(process); // sort된 결과 출력 
}

/*
* #14 - 1
*
* Preemptive Priority 스케줄링에서  
* 스케줄러에게 주어진 프로세스의 Burst time을 줄이기 전, 
* 다른 프로세스들 중에서 현재 time보다 작은 Arrival time을 가진
* 프로세스가 있는지를 체크해서
* 없다면 -1을 리턴,
* 있다면 그 프로세스의 index를 리턴 
*
* @param : process, time
*
*/
int CheckOtherPTime(Process *process, int current_index, int time){
	int i;
	
	for(i = 0 ; i < current_index + 1 ; i++){
		// 모든 인덱스에 대해서 검색안하는 이유 
		//- 이미 Priority에 대해서 정렬되어있기 때문에
		// i를 0에서부터 증가시키며 프로세스를 체크하다가
		// 현재 time보다 작은 arrival time의 프로세스를 찾으면
		// 그 프로세스의 index를 return! 
		
		if(process[i].ArrivalTime <= time){
			if(i == current_index){
				return i;
			}else{
				return i;
			}
		}

		
	}
	
	return -1;
} 




/*
* #14 
* 
* Preemptive Priority 스케줄링 
*  
* 
*/
void Run_P_Priority(Process *process){
	int j, i, time = 0;
	int check = 0; // ** 다른 프로세스들 중에 time과 Arrival Time이 같은 프로세스가 있는지 check. 
	int stop = 0; 
	
	Queue terminateQueue;
	
	InitQueue(&terminateQueue);
	
	printf("\n");
	
	while(terminateQueue.count != numOfProcess){
		stop = 0;
		
		printf("line181\n"); // for debugging
		
		for(int i = 0 ; i < numOfProcess ; i++){
			
			if(process[i].ArrivalTime <= time && process[i].BurstTimeCPU != 0){
				
				//
				while(process[i].BurstTimeCPU != 0){
					printf("line 190\n");
					check = CheckOtherPTime(process, i, time); // 실제로 time을 증가시키는 부분 이전에 나오도록?. 
					printf("check : %d\n",check);
					
					if(check < 0){ // 그냥 보통의 경우
						process[i].BurstTimeCPU--;
						time++;
					}else{
						i = check;
						process[i].BurstTimeCPU--;
						time++;
					}
			
				}
				//
				
				
				Enqueue(&terminateQueue, &process[i]);
				stop = 1; 
				
				i = numOfProcess; 
			}
		}
		
		if(stop == 0){ // 모든 프로세스를 검사해보았지만 time보다 작은 Arrival Time이 없을 때
			time++; // enqueue이후에는 이부분이 실행되면 안됨. 
			printf("line218 : time = %d\n",time);
		}
	} 	
	
	printf("\n");
	
	printf("Preemptive Priority scheduling finished!\n\n");
	
	ShowProcess(process);
	
	printf("total running time : %d \n", time); 
}



/*
* #13 0601 수정
* 
* Non-Preemptive Priority 스케줄링 
* 
*
* int stop : 0 
* terminate queue에 종료된 프로세스가 추가되었으니 
* 다시 한 번 line 162의 time을 increment할 필요가 없음을 의미 
*
* time - 실제 걸린 시간보다 1이 더 크게 나옴. 
* 
*/
void Run_NP_Priority(Process *process){
	int j, i, time = 0;
	int stop = 0; 
	
	int gant_chart[100] = {0, } ;
	
	Queue terminateQueue;
	
	InitQueue(&terminateQueue);
	
	printf("\n");
	
	// save #1
	
	// burst time 끝난 프로세스를 terminate queue 에 집어넣고,
	// terminate queue 에 있는 프로세스의 개수가 
	// 처음의 ready queue의 개수와 같아지면 종료
	while(terminateQueue.count != numOfProcess){
		stop = 0;

		printf("line 260\n"); // for debugging
		
		for(int i = 0 ; i < numOfProcess ; i++){
			
			if(process[i].ArrivalTime <= time && process[i].BurstTimeCPU != 0){
				
				while(process[i].BurstTimeCPU != 0){
					/////////////////waiting time 구하기/////////////////////
					if(process[i].WaitingTime == 0){
						process[i].WaitingTime = time - process[i].ArrivalTime; 
					}
					
					process[i].BurstTimeCPU--;
					gant_chart[time] = process[i].PID;
					time++;
					printf("270 : time = %d\n",time); // for debugging
				
				}

				// process[i]의 CPU burst time이 0이 되었으므로 terminate queue로 집어넣음. 
				Enqueue(&terminateQueue, &process[i]); // terminateQueue->count++ 
				stop = 1; // terminate queue에 추가되었으니 line 162의 increment가 일어나면 안됨. 
				
				printf("process[%d] has finished!!! \n",i); // for debugging
				printf("count of terminate queue : %d \n", terminateQueue.count); // for debugging
				
				i = numOfProcess; // for문을 벗어나서 다시 i = 0 부터 시작하기 위해서. line 154로 이동 
			}
		}
		
		if(stop == 0){
			gant_chart[time] = 71;
			time++; // enqueue이후에는 이부분이 실행되면 안됨. 
			printf("287 : time = %d\n", time); // for debugging
		}
	} 	
	
	printf("\n");
	
	printf("Non-Preemptive Priority scheduling finished!\n\n");
	
	ShowProcess(process);
	
	printf("total running time : %d \n", time); 
	
	// 간트차트 출력
	i = 0;
	while(gant_chart[i] != '\0'){
		printf("%2d|",gant_chart[i]);
		i++;
	}
	printf("\n");
	i = 0;
	// 시간 x축 출력 
	while(gant_chart[i] != '\0'){
		printf("%2d|",i+1);
		i++;
	}
	
	// Evaluation
	float awt = 0; // 함수로 바뀔 수 있으니 여기서 선언
	for(i = 0 ; i < numOfProcess ; i++){
		awt += process[i].WaitingTime;
	}
	printf("\nAverage Waiting time : %f\n", awt/numOfProcess);
	
}

/*
* #12
* FCFS 스케줄링 
* 주어진 프로세스(ArrivalTime으로 정렬된 프로세스)들을
* reqdyQueue에서 불러온 뒤, time을 1씩 증가시키면서 'CPU' Burst Time도 1씩 감소시킨다
* 그러다가도 중간에 I/O Exception을 발생시켜서 waitingQueue에 집어넣는다
* waitingQueue에서는 내 임의대로 FCFS방식으로 'I/O' BurstTime을 1씩 감소시킨다. 
*
*/
void Run_FCFS(Process *process){
	int i, time = 0; // 전체 진행 시간 
	int stop = 0;
	
	int gant_chart[100] = {0, };
	
	Queue terminateQueue;
	
	InitQueue(&terminateQueue);
	
	printf("\n");
	
	while(terminateQueue.count != numOfProcess){
		stop = 0;
		
		for(i = 0 ; i < numOfProcess ; i++){
			
			if(process[i].ArrivalTime <= time && process[i].BurstTimeCPU != 0){
				
				while(process[i].BurstTimeCPU != 0){
					/*waiting time 구하기*/
					if(process[i].WaitingTime == 0){
						process[i].WaitingTime = time - process[i].ArrivalTime;
					}
					
					process[i].BurstTimeCPU--; // CPU Burst time 감소
					gant_chart[time] = process[i].PID; 
					time++; // 전체 진행 시간 1 증가. 
				}
				
				// Turnaround time
				process[i].TurnAroundTime = time - process[i].ArrivalTime;
				
				Enqueue(&terminateQueue, &process[i]);
				stop = 1;
				
				printf("process[%d] has finished! \n", i);
				printf("count of terminate queue : %d \n", terminateQueue.count);
				
				i = numOfProcess;
			}
		}
		
		if(stop == 0){
			gant_chart[time] = 71;
			time++;
			printf("time = %d\n", time);
		}
	}
	
	printf("\nFCFS scheduling finshed!\n\n");
	
	ShowProcess(process); // 실행 결과 출력
	
	printf("total running time : %d \n", time);
	
	// 간트차트 출력
	i = 0;
	while(gant_chart[i] != '\0'){
		printf("%2d|",gant_chart[i]);
		i++;
	}
	printf("\n");
	i = 0;
	// 시간 x축 출력 
	while(gant_chart[i] != '\0'){
		printf("%2d|",i+1);
		i++;
	}
	// Evaluation
	float awt = 0; // 함수로 바뀔 수 있으니 여기서 선언
	float tat = 0;
	for(i = 0 ; i < numOfProcess ; i++){
		awt += process[i].WaitingTime;
		tat += process[i].TurnAroundTime;
	}
	printf("\nAverage Waiting time : %f\n", awt/numOfProcess);
	printf("Average Turnaround time : %f\n", tat/numOfProcess);
}

/*
* #11
* Print Menu Screen & Decide what algorithm to use.
* @param : null
* 20170529
*
*/
void PrintMenu(Queue *queue){
	int select_mode, i;
	int numOfProcess = queue->count;
	
	printf("Now you made processes.\nSo, What algorithm you want to apply?\n\n");
	
	printf("[1] First Come, First Served\n");
	printf("[2] Non-Preemptive Shortest Job First\n");
	printf("[3] Preemptive Shortest Job First\n");
	printf("[4] Non-Preemptive Priority\n");
	printf("[5] Preemptive Priority\n");
	printf("[6] Round Robin\n");
	
	printf("\nEnter the algorithm number you want! : ");
	scanf("%d", &select_mode);
	
	switch(select_mode){
		// FCFS
		case 1:
			
			CopyProcess2Queue(&queue_FCFS, process_FCFS);// 나중에 Run_FCFS 내부에 넣기! 
			sort_Arrival_Time(process_FCFS); // 나중에 Run_FCFS 내부에 넣기! 
			Run_FCFS(process_FCFS);
			
			break;
		
		// Non-Preemptive Shortest Job First	
		case 2:
			
			CopyProcess2Queue(&queue_NP_SJF, process_NP_SJF);
			sort_BurstTime(process_NP_SJF);
			Run_NP_SJF(process_NP_SJF);
			
			break;
			
		case 3:
			// TODO : Preemptive Shortest Job First
			for(i = 0 ; i < numOfProcess ; i++){
				Enqueue(&queue_P_SJF, process_P_SJF+i);
				printf("process[%d] was copied into Preemptive Priority queue!\n", i);
			}
			break;
			
		// Non-Preemptive Priority
		case 4:		
			
			CopyProcess2Queue(&queue_NP_Priority, process_NP_Priority);
			sort_Priority(process_NP_Priority); // numOfNode개의 프로세스들을 Non-Preemptive Priority에 맞게 정렬(priority) 
			Run_NP_Priority(process_NP_Priority);//
			
			break;
			
		case 5:
			// TODO : Preemptive Priority
			
			CopyProcess2Queue(&queue_P_Priority, process_P_Priority);
			sort_Priority(process_P_Priority); // 여기선 어떻게 sort해야하지?
			Run_P_Priority(process_P_Priority); // TODO 
			
			break;
			
		case 6:	
			// TODO : Round Robin
			
			CopyProcess2Queue(&queue_RR, process_RR);
			//sort_Arrival_Time(process_RR);
			//Run_RR(process_RR);
			
			break;
			
		default:
			printf("Please enter between 1 and 6 !\n");
			break;
	}
}

/*
* #10
* 주어진 프로세스들을 queue에 enqueue하는 함수 
* @param : queue, process
* 
*
*/
void CopyProcess2Queue(Queue *queue, Process *process){
	int i;
	
	for(i = 0 ; i < numOfProcess ; i++){
		Enqueue(queue, &process[i]);
	}
	
}

/*
* #9
* 프로세스와 프로세스의 개수를 인자로 받아서 
* 프로세스들의 arrival time 순으로 정렬해내는 함수 
* test하기 위하여  sort된 프로세스들을 출력(추후 삭제 필) 
*
*/
void sort_Arrival_Time(Process *process){
	Process temp_process;
	int i, j;
	
	for(i = 0 ; i < numOfProcess ; i++){
		for(j = 0 ; j < numOfProcess - 1 ; j++){
			if(process[j].ArrivalTime > process[j+1].ArrivalTime){ // arrival time 기준 정렬(오름차순)
				temp_process = process[j];
				process[j] = process[j+1];
				process[j+1] = temp_process;
			}
		}
	} 
	printf("\nGiven processes were sorted with Arrival Time!\n\n");
	ShowProcess(process); // sort된 결과 출력 
}

// #8
// ★★  Arrival time에 대해서도 sort되도록 구현해야!
void sort_Priority(Process *process){
	Process temp_process;
	int i, j;
	
	for(i = 0 ; i < numOfProcess ; i++){
		for(j = 0 ; j < numOfProcess - 1 ; j++){
			if(process[j].Priority > process[j+1].Priority){ // Priority 기준 정렬(내림차순)
				temp_process = process[j];
				process[j] = process[j+1];
				process[j+1] = temp_process;
			}
		}
	}
	
	// arrival time으로도 sorting
	// priority 같은게 2개보다 많으면?
	for(i = 0 ; i < numOfProcess - 1 ; i++){
		if(process[i].Priority == process[i+1].Priority){
			if(process[i].ArrivalTime > process[i+1].ArrivalTime){
				temp_process = process[i];
				process[i] = process[i+1];
				process[i+1] = temp_process;
			}
		}
	}
	printf("\nGiven processes were sorted with Priority!\n\n");
	ShowProcess(process); // sort된 결과 출력 
}

/*---------------------------------------------------*/

/*
* #7
* Initialize given queue
* @param : queue
*
*/
void InitQueue(Queue *queue){
	queue->front = queue->rear = NULL;
	queue->count = 0;
}

/*
* #6
* create processes & enqueue to the given queue.
* @param : queue
* 20170529
*
*/
Process* CreateProcess(Queue *queue){
	srand((unsigned int)time(NULL));
	int i;
	
	Process *process;
	
	process = (Process*)malloc(sizeof(Process));
	
	int numOfProcess = 4 + rand() % PROCESS_NUM_MAX;

	// Assigning data to the processes
	for(i = 0 ; i < numOfProcess ; i++){
		process[i].PID = 1 + rand() % 98;
		process[i].ArrivalTime = 1 + rand() % ARRIVAL_MAX; // hmm...
		process[i].BurstTimeCPU = 3 + rand() % BURST_MAX;
		process[i].BurstTimeIO = 3 + rand() % BURST_MAX;
		process[i].Priority = 1 + rand() % PRIORITY_MAX; // 중복을 피하기 위해 큰 숫자로 설정. 
		process[i].WaitingTime = 0;
		process[i].TurnAroundTime = 0;
		
		Enqueue(queue, &process[i]);
	}
	
	return process;
}

// #5
void ShowProcess(Process *process){
	int i;
	printf(" process[  ]   PID    Arrival Time   CPU Burst Time   I/O Burst Time   Priority   Waiting Time   TurnAround Time\n");
	printf("----------------------------------------------------------------------------------------------------------------\n");
	
	for(i = 0 ; i < numOfProcess ; i++){
		printf(" process[%2d]   %2d        %2d\t         %2d\t           %2d\t          %2d\t       %2d\t       %2d\n"
		, i
		, process[i].PID
		, process[i].ArrivalTime
		, process[i].BurstTimeCPU
		, process[i].BurstTimeIO
		, process[i].Priority
		, process[i].WaitingTime
		, process[i].TurnAroundTime);
	}
	
	printf("----------------------------------------------------------------------------------------------------------------\n");

	printf("\n");
}

// #4
int IsQueueEmpty(Queue *queue){ return queue->count == 0; }

// #3
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

// #2
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


// #1 Copying processes for each scheduling algorithms.
 void  CopyProcess(Process *process){	 		
	// Copying @param - process
	process_FCFS = process;
	process_NP_SJF = process;
	process_P_SJF = process;
	process_NP_Priority = process;
	process_P_Priority = process;
	process_RR = process;
	
	return;
 }