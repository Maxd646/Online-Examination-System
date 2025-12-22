#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <iostream>
#include <stdexcept>

using namespace std;

template <typename T>
class LinkedList
{
private:
    struct Node
    {
        T data;
        Node *next;
        Node(const T &value)
        {
            data = value;
            next = nullptr;
        }
    };
    Node *head;
    Node *tail;
    size_t size;

public:
    LinkedList()
    {
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    ~LinkedList()
    {
        clear();
    }

    // Copy constructor
    LinkedList(const LinkedList &other) : head(nullptr), tail(nullptr), size(0)
    {
        Node *current = other.head;
        while (current)
        {
            push_back(current->data);
            current = current->next;
        }
    }

    // Assignment operator
    LinkedList &operator=(const LinkedList &other)
    {
        if (this != &other)
        {
            clear();
            Node *current = other.head;
            while (current)
            {
                push_back(current->data);
                current = current->next;
            }
        }
        return *this;
    }

    void push_back(const T &value)
    {
        Node *newNode = new Node(value);
        if (!head)
        {
            head = tail = newNode;
        }
        else
        {
            tail->next = newNode;
            tail = newNode;
        }
        size++;
    }

    void clear()
    {
        while (head)
        {
            Node *temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
        size = 0;
    }

    size_t getSize() const { return size; }
    bool empty() const { return size == 0; }
};

#endif // LINKED_LIST_H