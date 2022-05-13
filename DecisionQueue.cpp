#include "DecisionQueue.h"

/// @brief Initializer
/// @param c size of the queue
DecisionQueue::DecisionQueue(int c)
{
    front = rear = 0;
    capacity = c;
    queue = new double[c];
}

/// @brief Enqueues the values to the queue
/// @param data double value to enqueue
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

/// @brief Dequeues and removes first item
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

/// @brief Returns the count of the queue
/// @returns count of the queue
int DecisionQueue::getCount()
{
    return rear;
}

/// @brief Converts the queue to an array
/// @returns Data struct holding the queueu detail and length
DecisorData DecisionQueue::toArray()
{
    DecisorData data;
    data.data = queue;
    data.length = rear;
    return data;
}