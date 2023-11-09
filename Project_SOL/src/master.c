#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>

#include "master.h"
#include "workerPool.h"
#include "dirFiles.h"

#define DEBUG

#define PATH_MAXLEN 255

#define HANDLE_ERROR_QUEUE(message) \
	fprintf(stderr, "Error: %s\n", message); \
	_exit(EXIT_FAILURE)

#define PTHREAD_ERROR(mutex_ok, condFull_ok, condEmpty_ok, mutex, full, empty, message, errno) \
	mutex_ok ? pthread_mutex_destroy(&mutex);
	condFull_ok ? pthread_cond_destroy(&full);
	condEmpty_ok ? pthread_cond_destroy(&empty);
	fprintf(stderr, "Error: %s\n", message); \
	pthread_exit(errno)


typedef struct _concurrent_Queue{
	size_t capacity; //queue capacity (-q <qlen> : defined as main's argument or defaul)
	char** queue; //array that contains the filenames to give as workers' parameter
	pthread_mutex_t mutex; //mutex to guarantee mutua exclusion
	pthread_cond_t full;  //conditional variable that says if buffer is ready for enqueu 
	pthread_cond_t empty; //conditional variable that says if buffer is ready for dequeue
	unsigned int position; //index of the current position in the queue where enqueue or dequeue
	unsigned int currSize; //number of elements currently in the queue
}concurrentQueue;

void master(int threadsNum, int queueLen, char* dirName, int timeDelay,
				 	int filesNum, char** filesArr, concurrentQueue queue){

		#if defined(DEBUG)
		fprintf(stdout, "n: %d, q: %d, dirName: %s, t: %d, filesNum: %d\n", 
				threadsNum, queueLen, (dirName!=NULL) ? dirName : "DIRNAME", timeDelay, filesNum);
		fprintf(stdout, "filesArr:\n");
		for(unsigned int j=0; j<filesNum; j++){
			fprintf(stdout, "%d°: %s\n", j, filesArr[j]);
		}
		fprintf(stdout, "END MASTER DEBUG LINES");
		fprintf(stdout, "THE END");
		#endif

		if(dirName != NULL){
			exploreDir(dirName, filesArr, &filesNum);
		}

		concurrentQueue* queue = initQueue(queueLen);

		pthread_t th_master;
		pthread_t th_workers[threadsNum];
		if(pthread_create(th_master, NULL, &masterFun, 404, ) != 0)
		for(unsigned int i=0; i<threadsNum; i++){
			if(pthread_create(th[i], NULL, &workerFun, i, file) != 0)
		} 
}

void masterFun(char** filenameArr, char*)

void exploreDir(char* dirName, char** filesArr);

concurrentQueue* initQueue(size_t capacity){
	if(concurrentQueue <= 0){
		HANDLE_ERROR_QUEUE("Queue capacity is less then 1");
	}
	int err=0;
	concurrentQueue* queue = NULL;
	pthread_mutex_t mutex;
	pthread_cond_t full;
	pthread_cond_t empty;
	bool mutex_ok=false, condFull_ok=false, condEmpty_ok=false;
	char** filenameArr = NULL;
	if((err=pthread_mutex_init(&mutex, NULL)) != 0){
		PTHREAD_ERROR(mutex_ok, condFull_ok, condEmpty_ok, mutex, full, empty, "initilizing mutex 'mutex' in initQueue", errno);
	}
	else{
		mutex_ok = true;
	}
	if((err=pthread_cond_init(&full, NULL)) != 0){
		PTHREAD_ERROR(mutex_ok, condFull_ok, condEmpty_ok, mutex, full, empty, "initilizing conditional variable 'full' in initQueue", errno);
	}
	else{
		condFull_ok = true;
	}
	if((err=pthread_cond_init(&empty, NULL)) != 0){
		PTHREAD_ERROR(mutex_ok, condFull_ok, condEmpty_ok, mutex, full, empty, "initilizing conditional variable 'empty' in initQueue", errno);
	}
	else{
		condEmpty_ok = true;
	}

	filenameArr = (char**) malloc(capacity*sizeof(char*));
	//Insert a pthread_error???
	assert(filenameArr);
	for(unsigned int i=0; i<capacity; i++){
		filenameArr = (char*) calloc(PATH_MAXLEN, sizeof(char));
	}


	queue = (concurrentQueue) malloc(sizeof(concurrentQueue));
	queue->capacity = capacity;
	queue->queue = filenameArr;
	queue->mutex = mutex;
	queue->full = full;
	queue->empty = empty;
	queue->position = 0;
	queue->currSize = 0;

	return queue;
}

bool enqueueQueue(concurrentQueue* queue, char* filename){
	if(queue == NULL || filename == NULL){
		return false;
	}
	int err = 0; 

	if((err=pthread_mutex_lock(&(queue->mutex))) != 0){
		return false;
	}
	while(queue->currSize == queue->capacity){
		pthread_cond_wait(&(queue->full), &(queue->mutex));
	}

	//implements and check circular array
	queue->position = queue->position + 1;
	if((queue->position) == (queue->capacity)){
		queue->position = 0
	}
	queue.queue[(queue->position)] = strncpy(queue.queue[(queue->position)], filename, strnlen(filename, PATH_MAXLEN));  
	queue->currSize = queue->currSize + 1;
	
	if(queue->currSize > 0){
		pthread_cond_broadcast(&(queue->empty));
	}
	if((err=pthread_mutex_unlock(&(queue_mutex))) != 0){
		return false;
	}
	return true;
}

bool dequeueQueue(concurrentQueue* queue, char** filename){
	if(queue == NULL || filename == NULL){
		return false;
	}
	int err = 0; 
	int strLength = 0;
	char* str = NULL;

	if((err=pthread_mutex_lock(&(queue->mutex))) != 0){
		return false;
	}
	while(queue->currSize < 1){
		pthread_cond_wait(&(queue->empty), &(queue->mutex));
	}

	strLength =  strnlen(queue.queue[(queue->position)], PATH_MAXLEN);
	str = (char*) calloc(strnlen+1, sizeof(char*));
	assert(str);
	str = strncpy(queue.queue[queue->position], strLength);
	str[strLength] = "\0";
	&filename = strncpy(&filename, str, strLength+1);
	if(str != NULL){
		free(str);
	}
	
	//implements and check circular array
	queue->position = queue->position - 1;
	if(queue->position == -1){
		queue->position = queue->capacity - 1;
	}
	queue->currSize = queue->currSize - 1;
	
	if(queue->currSize < queue->capacity){
		pthread_cond_broadcast(&(queue->empty));
	}
	if((err=pthread_mutex_unlock(&(queue_mutex))) != 0){
		return false;
	}
	return true;
}