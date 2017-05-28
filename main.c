#include "CreateProcess.h"

int main(){
	int i;
	Queue queue;
	
	InitQueue(&queue);
	
	Process *process;
	process = CreateProcess(&queue);
	
	ShowProcess(&queue, process);
	
}

