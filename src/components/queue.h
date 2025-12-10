#ifndef QUEUE_H
#define QUEUE_H

#include <vector>
#include <stdexcept>

template<typename T>
class Queue {
private:
    std::vector<T> data;
    size_t frontIndex;
    
public:
    Queue() : frontIndex(0) {}
    
    void push(const T& value) {
        data.push_back(value);
    }
    
    void pop() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        
        frontIndex++;
        
        // Optimize memory usage - remove processed elements when queue gets large
        if (frontIndex > data.size() / 2 && frontIndex > 100) {
            data.erase(data.begin(), data.begin() + frontIndex);
            frontIndex = 0;
        }
    }
    
    T& front() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return data[frontIndex];
    }
    
    const T& front() const {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return data[frontIndex];
    }
    
    T& back() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return data.back();
    }
    
    const T& back() const {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return data.back();
    }
    
    bool empty() const {
        return frontIndex >= data.size();
    }
    
    size_t size() const {
        return data.size() - frontIndex;
    }
    
    void clear() {
        data.clear();
        frontIndex = 0;
    }
    
    // Advanced operations
    void display() const {
        std::cout << "Queue: [";
        for (size_t i = frontIndex; i < data.size(); ++i) {
            std::cout << data[i];
            if (i < data.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
    
    // Peek at element at specific position from front
    const T& peek(size_t position = 0) const {
        if (frontIndex + position >= data.size()) {
            throw std::out_of_range("Position out of range");
        }
        return data[frontIndex + position];
    }
    
    // Check if queue contains a specific value
    bool contains(const T& value) const {
        for (size_t i = frontIndex; i < data.size(); ++i) {
            if (data[i] == value) {
                return true;
            }
        }
        return false;
    }
    
    // Get all elements as vector (for iteration)
    std::vector<T> toVector() const {
        return std::vector<T>(data.begin() + frontIndex, data.end());
    }
};

// Priority Queue implementation
template<typename T, typename Compare = std::less<T>>
class PriorityQueue {
private:
    std::vector<T> heap;
    Compare comp;
    
    void heapifyUp(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (!comp(heap[index], heap[parent])) {
                break;
            }
            std::swap(heap[index], heap[parent]);
            index = parent;
        }
    }
    
    void heapifyDown(size_t index) {
        size_t size = heap.size();
        
        while (true) {
            size_t smallest = index;
            size_t left = 2 * index + 1;
            size_t right = 2 * index + 2;
            
            if (left < size && comp(heap[left], heap[smallest])) {
                smallest = left;
            }
            
            if (right < size && comp(heap[right], heap[smallest])) {
                smallest = right;
            }
            
            if (smallest == index) {
                break;
            }
            
            std::swap(heap[index], heap[smallest]);
            index = smallest;
        }
    }
    
public:
    PriorityQueue() {}
    
    void push(const T& value) {
        heap.push_back(value);
        heapifyUp(heap.size() - 1);
    }
    
    void pop() {
        if (empty()) {
            throw std::runtime_error("Priority queue is empty");
        }
        
        heap[0] = heap.back();
        heap.pop_back();
        
        if (!empty()) {
            heapifyDown(0);
        }
    }
    
    const T& top() const {
        if (empty()) {
            throw std::runtime_error("Priority queue is empty");
        }
        return heap[0];
    }
    
    bool empty() const {
        return heap.empty();
    }
    
    size_t size() const {
        return heap.size();
    }
    
    void clear() {
        heap.clear();
    }
    
    void display() const {
        std::cout << "Priority Queue: [";
        for (size_t i = 0; i < heap.size(); ++i) {
            std::cout << heap[i];
            if (i < heap.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
};

#endif // QUEUE_H