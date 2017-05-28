#include "CreateProcess.h"

int main(){
	int i;
	Queue readyQueue, copiedQueue;
	
	InitQueue(&readyQueue);
	InitQueue(&copiedQueue);
	
	Process *process;
	
	process = CreateProcess(&readyQueue);
	
	copiedQueue = readyQueue;
	
	ShowProcess(&readyQueue, process);
	printf("%d\n", copiedQueue.front->data->PID);
}

