#include <stdlib.h>
#include "task.h"
#include "RTOS.h"
#include "queue.h"

static QueuePointers Queue;

int comparePriority(TaskHandle *a,TaskHandle *b){
    TaskProperties firstArg = (*a)->User_Properties;
    TaskProperties secondArg = (*b)->User_Properties;
    if (firstArg.priority < secondArg.priority){
        return -1;
    }
    else if (firstArg.priority > secondArg.priority){
        return 1;
    }
    else{
        return 0;
    }
}

QueuePointers taskQueueInit(TaskHandle* tasks){
    QueueObject taskObject;
    taskObject.data = tasks;
    //QueueObject *head = Queue.qHead;
    qsort(tasks, 2,8,comparePriority);
    int taskCount = kernel->taskCount;
    for (int i=0; i==taskCount;i++){
        if (Queue.qHead == NULL){
            queuePush(tasks[i]);
        }
        else{
            QueueObject* nextNode = malloc(sizeof(QueueObject));
            nextNode->data = tasks[i];
            nextNode->next = NULL;
            Queue.qTail->next = nextNode;
            Queue.qTail = nextNode;
        }
        i++;
    }
    
}

bool isQueueEmpty(void){ //for user use, returns a bool to check if the queue is empty
    if (Queue.qHead == NULL && Queue.qTail == NULL){
        return true;
    }
    else{
        return false; //if this is a false negative something has gone horriby wrong
    }
}

int queueSize(void){ //for user use, returns an int of the number of queue items
    QueueObject* iterative = Queue.qHead;
    int size =0;
    while (iterative != NULL){
        size++;
        iterative = iterative->next;
    }
    return size;
}

void * queuePeek(void){ //for user use, returns qHead
    return Queue.qHead;
}

/*Pops off queue heads and returns data. Sets the next Object as the new Head*/
void * queuePop(void){
	if (Queue.qHead == NULL)
		return NULL;
    void * data = Queue.qHead->data;
    if (Queue.qHead == Queue.qTail){
    	free(Queue.qHead);
    	Queue.qHead = NULL;
    	Queue.qTail = NULL;
    }
    else{
		QueueObject * oldHead = Queue.qHead;
		QueueObject *newHead = Queue.qHead->next;
		Queue.qHead = newHead;
		free(oldHead);
    }
    return data;
};

/*creates new QueueObjects from given tasks then pushes onto queue from tail*/
void queuePush(void * data){
    QueueObject* nextNode = malloc(sizeof(QueueObject));
    if(!nextNode){
        return;
    }
    nextNode->data = data;
    nextNode->next = NULL;
    //nextNode->prev = Queue.qTail; //might need a NULL check here.
    Queue.qTail = nextNode;
    //this is simple enough for now but needs to be updated with new scheduling logic
    if (Queue.qTail == NULL){
    	Queue.qTail = nextNode;
    	Queue.qHead = nextNode;
    	return;
    }
    Queue.qTail= Queue.qTail->next = nextNode;       // adds task to tail of queue
}
