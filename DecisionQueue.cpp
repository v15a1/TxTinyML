#include "DecisionQueue.h"

DecisionQueue::DecisionQueue(int c)
{
    front = rear = 0;
    capacity = c;
    queue = new double[c];
}

void DecisionQueue::enqueue(double data)
{
    // check queue is full or not
    if (capacity == rear)
    {
        dequeue();
    }
    else
    {
        queue[rear] = data;
        rear++;
    }
    return;
}

void DecisionQueue::dequeue()
{
    // if queue is empty
    if (front == rear)
    {
        return;
    }

    // shift all the elements from index 2 till rear
    // to the left by one
    else
    {
        for (int i = 0; i < rear - 1; i++)
        {
            queue[i] = queue[i + 1];
        }
        rear--;
    }
    return;
}

int DecisionQueue::getCount()
{
    return rear;
}