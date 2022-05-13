#ifndef PROXIMITY_DECISION_QUEUE_H_
#define PROXIMITY_DECISION_QUEUE_H_

#include "DecisorData.h"

class DecisionQueue
{
public:
    DecisionQueue(int c);
    void enqueue(double data);
    void dequeue();
    int getCount();
    DecisorData toArray();

private:
    int front, rear, capacity;
    double *queue;
};
#endif