#include <stdlib.h>
#include "task.h"
#include "RTOS.h"
#include "queue.h"

static QueuePointers Queue;

/*Pops off queue heads and returns data. Sets the next Object as the new Head*/
TaskHandle QueuePop(){
    TaskHandle data = Queue.qHead->data;
    QueueObject *newHead = Queue.qHead->next;
    Queue.qHead = newHead;
    return data;
};

/*creates new QueueObjects from given tasks then pushes onto queue from tail*/
void QueuePush(TaskHandle task){
    QueueObject* nextTask = malloc(sizeof(QueueObject));
    if(!nextTask){
        return;
    }
    nextTask->data = task;
    nextTask->next = NULL;
    //this is simple enough for now but needs to be updated with new scheduling logic
    Queue.qTail->next = nextTask;       // adds task to tail of queue
    Queue.qTail = nextTask;             // moves tail to next task
}
