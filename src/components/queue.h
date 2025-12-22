#ifndef QUEUE_H
#define QUEUE_H

#include <vector>
#include <stdexcept>

using namespace std;

template <typename T>
class Queue
{
private:
    vector<T> data;
    size_t frontIndex;

public:
    Queue()
    {
        frontIndex = 0;
    }

    void push(const T &value)
    {
        data.push_back(value);
    }

    void pop()
    {
        if (empty())
        {
            throw runtime_error("Queue is empty");
        }

        frontIndex++;

        // Optimize memory usage by removing processed elements
        if (frontIndex > data.size() / 2 && frontIndex > 100)
        {
            data.erase(data.begin(), data.begin() + frontIndex);
            frontIndex = 0;
        }
    }

    T &front()
    {
        if (empty())
        {
            throw runtime_error("Queue is empty");
        }
        return data[frontIndex];
    }

    const T &front() const
    {
        if (empty())
        {
            throw runtime_error("Queue is empty");
        }
        return data[frontIndex];
    }

    bool empty() const
    {
        return frontIndex >= data.size();
    }

    size_t size() const
    {
        return data.size() - frontIndex;
    }

    void clear()
    {
        data.clear();
        frontIndex = 0;
    }
};

#endif // QUEUE_H