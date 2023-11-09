#include <stdio.h>
#include "workerPool.h"

void workerFun(unsigned int thIndex, char* filename){
	fprintf(stdout, "I'm in the worker with id %d\n", tid);
	fprintf(stdout, "This is the file with name : %s\n", filename);
}