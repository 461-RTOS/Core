#include <stdlib.h>
#include "task.h"
#include "RTOS.h"
#include "queue.h"

static QueuePointers Queue;

/*Pops off queue heads and returns data. Sets the next Object as the new Head*/
void * QueuePop(void){
	if (Queue.qHead == NULL)
		return NULL;
    void * data = Queue.qHead->data;
    QueueObject * oldHead = Queue.qHead;
    QueueObject *newHead = Queue.qHead->next;
    Queue.qHead = newHead;
    free(oldHead);
    return data;
};

/*creates new QueueObjects from given tasks then pushes onto queue from tail*/
void QueuePush(void * data){
    QueueObject* nextNode = malloc(sizeof(QueueObject));
    if(!nextNode){
        return;
    }
    nextNode->data = data;
    nextNode->next = NULL;
    //this is simple enough for now but needs to be updated with new scheduling logic
    if (Queue.qTail == NULL){
    	Queue.qTail = nextNode;
    	Queue.qHead = nextNode;
    	return;
    }
    Queue.qTail= Queue.qTail->next = nextNode;       // adds task to tail of queue
}
