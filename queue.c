#include <stdlib.h>
#include "task.h"
#include "RTOS.h"
#include "queue.h"

static QueuePointers Queue;


/** Following function is needed for qsort to sort our array of tasks*/
int comparePriority(const void * a, const void * b){
    TaskProperties firstArg = ((TaskHandle)a)->User_Properties; //a bit weird but the pointers need to be dereferenced
    TaskProperties secondArg = ((TaskHandle)b)->User_Properties;
    if (firstArg.priority < secondArg.priority){ //according to qsort we need a negative integer for less
        return -1;
    }
    else if (firstArg.priority > secondArg.priority){//positive for more
        return 1;
    }
    else{//0 for equal
        return 0;
    }
}
/** Function Initializing the task Queue **/
QueuePointers* taskQueueInit(TaskHandle* tasks){
    QueuePointers* taskQueue = malloc(sizeof(QueuePointers));
    int taskCount = kernel->taskCount; //sets a variable for the number of tasks
    qsort(tasks, taskCount, sizeof(TaskHandle), comparePriority);// calling qsort to sort the array		// Changed arg3 from 8 to sizeof(TaskHandle) NEVER USE A CONSTANT FOR A SIZE - Use sizeof(instead), (especially because the size of a pointer on our platform is 4 not 8)
    for (int i=0; i==taskCount;i++){
        if (taskQueue->qHead == NULL){
            queuePush(tasks[i],taskQueue); //check for a NULL head
        }
        else{
            QueueObject* nextNode = malloc(sizeof(QueueObject));// setting aside memeory and then placing elements into the queue
            nextNode->data = tasks[i];
            nextNode->next = NULL;
            taskQueue->qTail->next = nextNode;
            taskQueue->qTail = nextNode;
        }
        i++;
    }
    //QueuePointers taskQueue  = Queue;
    return taskQueue;
}
												
bool isQueueEmpty(QueuePointers* userQueue){ //for user use, returns a bool to check if the queue is empty
    if (userQueue->qHead == NULL && userQueue->qTail == NULL){
        return true;
    }
    else{
        return false; //if this is a false negative something has gone horriby wrong
    }
}

int queueSize(QueuePointers * userQueue){ //for user use, returns an int of the number of queue items
    QueueObject* iterative = userQueue->qHead;
    unsigned int size =0;
    while (iterative != NULL){
        size++;
        iterative = iterative->next;
    }
    return size;
}

void * queuePeek(QueuePointers* userQueue){ //for user use, returns qHead
    return userQueue->qHead;
}

/*Pops off queue heads and returns data. Sets the next Object as the new Head*/
void * queuePop(QueuePointers* userQueue){
	if (userQueue->qHead == NULL)
		return NULL;
    void * data = Queue.qHead->data;
    if (userQueue->qHead == Queue.qTail){
    	free(userQueue->qHead);
    	userQueue->qHead = NULL;
    	userQueue->qTail = NULL;
    }
    else{
		QueueObject * oldHead = userQueue->qHead;
		QueueObject *newHead = userQueue->qHead->next;
		userQueue->qHead = newHead;
		free(oldHead);
    }
    return data;
};

/*creates new QueueObjects from given tasks then pushes onto queue from tail*/
void queuePush(void * data, QueuePointers* userQueue){
    QueueObject* nextNode = malloc(sizeof(QueueObject));
    if(!nextNode){
        return;
    }
    nextNode->data = data;
    nextNode->next = NULL;
    //nextNode->prev = Queue.qTail; //might need a NULL check here.
    userQueue->qTail = nextNode;
    //this is simple enough for now but needs to be updated with new scheduling logic
    if (userQueue->qTail == NULL){
    	userQueue->qTail = nextNode;
    	userQueue->qHead = nextNode;
    	return;
    }
    userQueue->qTail= userQueue->qTail->next = nextNode;       // adds task to tail of queue
}
