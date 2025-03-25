#include <stdlib.h>
#include "task.h"
#include "RTOS.h"
#include "queue.h"

static QueuePointers Queue;

void taskQueueInit(TaskHandle task){
    //QueueObject taskObject;
    //taskObject.data = task;
    //QueueObject *head = Queue.qHead;
    if (Queue.qHead == NULL){
        QueuePush(task);
    }
    else if (((TaskHandle)(Queue.qHead->data))->User_Properties.priority <= task->User_Properties.priority){
        QueueObject* taskPointer=malloc(sizeof(QueueObject));
        if(!taskPointer){
            return;
        }
        taskPointer->data = task;
        taskPointer->next = NULL;
        taskPointer->prev = Queue.qHead;
    }
    else if (((TaskHandle)(Queue.qHead->data))->User_Properties.priority > task->User_Properties.priority){
        QueueObject* taskPointer=malloc(sizeof(QueueObject));
        if (!taskPointer){
            return;
        }
        taskPointer->data = task;
        taskPointer->next = Queue.qHead;
        taskPointer->prev = NULL;
    }
}

/*Pops off queue heads and returns data. Sets the next Object as the new Head*/
void * QueuePop(void){
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
void QueuePush(void * data){
    QueueObject* nextNode = malloc(sizeof(QueueObject));
    if(!nextNode){
        return;
    }
    nextNode->data = data;
    nextNode->next = NULL;
    nextNode->prev = Queue.qTail; //might need a NULL check here.
    Queue.qTail = nextNode;
    //this is simple enough for now but needs to be updated with new scheduling logic
    if (Queue.qTail == NULL){
    	Queue.qTail = nextNode;
    	Queue.qHead = nextNode;
    	return;
    }
    Queue.qTail= Queue.qTail->next = nextNode;       // adds task to tail of queue
}
