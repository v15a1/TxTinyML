#ifndef PROXIMITY_DECISION_QUEUE_H_
#define PROXIMITY_DECISION_QUEUE_H_

class DecisionQueue
{
public:
    DecisionQueue(int c);
    void enqueue(double data);
    void dequeue();
    void printQueue();
    int getCount();

private:
    int front, rear, capacity;
    double *queue;
};
#endif