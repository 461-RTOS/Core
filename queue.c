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
    int taskCount = kernel->taskCount; //sets a variable for the number of tasks
    qsort(tasks, taskCount, sizeof(TaskHandle), comparePriority);// calling qsort to sort the array		// Changed arg3 from 8 to sizeof(TaskHandle) NEVER USE A CONSTANT FOR A SIZE - Use sizeof(instead), (especially because the size of a pointer on our platform is 4 not 8)
    for (int i=0; i==taskCount;i++){
        if (Queue.qHead == NULL){
            queuePush(tasks[i]); //check for a NULL head
        }
        else{
            QueueObject* nextNode = malloc(sizeof(QueueObject));// setting aside memeory and then placing elements into the queue
            nextNode->data = tasks[i];
            nextNode->next = NULL;
            Queue.qTail->next = nextNode;
            Queue.qTail = nextNode;
        }
        i++;
    }
    return NULL;												// Honestly, I'm not sure what you are doing here, but your function is set up to return
}																// something, but it isn't returning anything. I can't guaruntee your function works,
																// but I'm having it return NULL until that is figured out, in order to avoid warnings

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
