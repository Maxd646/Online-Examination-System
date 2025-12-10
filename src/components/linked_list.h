#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <iostream>
#include <functional>

template<typename T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& value) : data(value), next(nullptr) {}
    };
    
    Node* head;
    Node* tail;
    size_t size;

public:
    LinkedList() : head(nullptr), tail(nullptr), size(0) {}
    
    ~LinkedList() {
        clear();
    }
    
    // Copy constructor
    LinkedList(const LinkedList& other) : head(nullptr), tail(nullptr), size(0) {
        Node* current = other.head;
        while (current) {
            push_back(current->data);
            current = current->next;
        }
    }
    
    // Assignment operator
    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            clear();
            Node* current = other.head;
            while (current) {
                push_back(current->data);
                current = current->next;
            }
        }
        return *this;
    }
    
    void push_back(const T& value) {
        Node* newNode = new Node(value);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        size++;
    }
    
    void push_front(const T& value) {
        Node* newNode = new Node(value);
        if (!head) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head = newNode;
        }
        size++;
    }
    
    bool remove(const T& value) {
        if (!head) return false;
        
        if (head->data == value) {
            Node* temp = head;
            head = head->next;
            if (!head) tail = nullptr;
            delete temp;
            size--;
            return true;
        }
        
        Node* current = head;
        while (current->next && current->next->data != value) {
            current = current->next;
        }
        
        if (current->next) {
            Node* temp = current->next;
            current->next = temp->next;
            if (temp == tail) tail = current;
            delete temp;
            size--;
            return true;
        }
        
        return false;
    }
    
    T* find(const T& value) {
        Node* current = head;
        while (current) {
            if (current->data == value) {
                return &(current->data);
            }
            current = current->next;
        }
        return nullptr;
    }
    
    void clear() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
        size = 0;
    }
    
    size_t getSize() const { return size; }
    bool empty() const { return size == 0; }
    
    T& front() {
        if (!head) throw runtime_error("List is empty");
        return head->data;
    }
    
    T& back() {
        if (!tail) throw runtime_error("List is empty");
        return tail->data;
    }
    
    void pop_front() {
        if (head) {
            Node* temp = head;
            head = head->next;
            if (!head) tail = nullptr;
            delete temp;
            size--;
        }
    }
    
    void pop_back() {
        if (!head) return;
        
        if (head == tail) {
            delete head;
            head = tail = nullptr;
            size--;
            return;
        }
        
        Node* current = head;
        while (current->next != tail) {
            current = current->next;
        }
        
        delete tail;
        tail = current;
        tail->next = nullptr;
        size--;
    }
    
    // Iterator support
    class Iterator {
    private:
        Node* current;
    public:
        Iterator(Node* node) : current(node) {}
        
        T& operator*() { return current->data; }
        Iterator& operator++() { current = current->next; return *this; }
        bool operator!=(const Iterator& other) { return current != other.current; }
        bool operator==(const Iterator& other) { return current == other.current; }
    };
    
    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }
    
    // Advanced operations
    void reverse() {
        if (!head || !head->next) return;
        
        Node* prev = nullptr;
        Node* current = head;
        tail = head;
        
        while (current) {
            Node* next = current->next;
            current->next = prev;
            prev = current;
            current = next;
        }
        
        head = prev;
    }
    
    void sort() {
        if (size <= 1) return;
        
        // Simple bubble sort for linked list
        bool swapped;
        do {
            swapped = false;
            Node* current = head;
            
            while (current && current->next) {
                if (current->data > current->next->data) {
                    swap(current->data, current->next->data);
                    swapped = true;
                }
                current = current->next;
            }
        } while (swapped);
    }
    
    template<typename Predicate>
    void removeIf(Predicate pred) {
        while (head && pred(head->data)) {
            Node* temp = head;
            head = head->next;
            delete temp;
            size--;
        }
        
        if (!head) {
            tail = nullptr;
            return;
        }
        
        Node* current = head;
        while (current->next) {
            if (pred(current->next->data)) {
                Node* temp = current->next;
                current->next = temp->next;
                if (temp == tail) tail = current;
                delete temp;
                size--;
            } else {
                current = current->next;
            }
        }
    }
    
    void display() const {
        Node* current = head;
        cout << "[";
        while (current) {
            cout << current->data;
            if (current->next) cout << " -> ";
            current = current->next;
        }
        cout << "]" << endl;
    }
};

#endif // LINKED_LIST_H