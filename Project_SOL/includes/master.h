#ifndef MASTER_H_
#define MASTER_H_

#include <stdbool.h>

typedef struct _concurrent_Queue concurrentQueue;

/**
 * @brief: Function used for MasterWorker thread
 * @requires: concurrentQueue != NULL
 * @effects: produce values in concurrentQueue for the workers threads
 * @throws: if there's a invalid pathname for a file then terminate the program
*/
void master(int threadsNum, int queueLen, char* dirName, int timeDelay,
				 	int filesNum, char** filesArr, concurrentQueue queue);

/**
 * @brief: Initialize the queue with the given capacity
 * @requires: capacity > 0
 * @effects: return a concurrentQueue that will be used as "the queue of the task to perform"
 * @throws: if capacity < 0, then prints an ERROR_MESSAGE
*/
concurrentQueue* initQueue(size_t capacity);

/**
 * @brief: Inserts an element in the queue
 * @requires: queue != NULL && filename != NULL
 * @effects: modifies concurrentQueue, it inserts 'filename' string in the queue;
 * 			 return true if enqueue has been executed successfully, false otherwise
 * @throws: if queue or filename are null then prints an ERROR_MESSAGE 
 * 			and terminates the program.
*/
bool enqueueQueue(concurrentQueue* queue, char* filename);

/**
 * @brief: Removes an element from the queue
 * @requires: queue != NULL && filename != NULL
 * @effects: modifies concurrentQueue, it removes an element from the queue and 
 * 			 assign it to filename pointer;
 * 			 return true if dequeue has been executed successfully, false otherwise
 * @throws: if queue or filename are null then prints an ERROR_MESSAGE 
 * 			and terminates the program.
*/
bool dequeueQueue(concurrentQueue* queue, char** filename);

#endif