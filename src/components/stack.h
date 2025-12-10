#ifndef STACK_H
#define STACK_H

#include <vector>
#include <stdexcept>

template<typename T>
class Stack {
private:
    std::vector<T> data;
    
public:
    Stack() {}
    
    void push(const T& value) {
        data.push_back(value);
    }
    
    void pop() {
        if (empty()) {
            throw std::runtime_error("Stack is empty");
        }
        data.pop_back();
    }
    
    T& top() {
        if (empty()) {
            throw std::runtime_error("Stack is empty");
        }
        return data.back();
    }
    
    const T& top() const {
        if (empty()) {
            throw std::runtime_error("Stack is empty");
        }
        return data.back();
    }
    
    bool empty() const {
        return data.empty();
    }
    
    size_t size() const {
        return data.size();
    }
    
    void clear() {
        data.clear();
    }
    
    // Advanced operations
    void display() const {
        std::cout << "Stack (top to bottom): [";
        for (int i = data.size() - 1; i >= 0; --i) {
            std::cout << data[i];
            if (i > 0) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
    
    // Peek at element at specific position from top
    const T& peek(size_t position = 0) const {
        if (position >= data.size()) {
            throw std::out_of_range("Position out of range");
        }
        return data[data.size() - 1 - position];
    }
    
    // Check if stack contains a specific value
    bool contains(const T& value) const {
        for (const auto& item : data) {
            if (item == value) {
                return true;
            }
        }
        return false;
    }
    
    // Get all elements as vector (bottom to top order)
    std::vector<T> toVector() const {
        return data;
    }
    
    // Get all elements in reverse order (top to bottom)
    std::vector<T> toVectorReverse() const {
        std::vector<T> result;
        for (int i = data.size() - 1; i >= 0; --i) {
            result.push_back(data[i]);
        }
        return result;
    }
    
    // Swap top two elements
    void swapTop() {
        if (size() < 2) {
            throw std::runtime_error("Stack must have at least 2 elements to swap");
        }
        std::swap(data[data.size() - 1], data[data.size() - 2]);
    }
    
    // Duplicate top element
    void duplicate() {
        if (empty()) {
            throw std::runtime_error("Cannot duplicate from empty stack");
        }
        push(top());
    }
    
    // Rotate stack (move top element to bottom)
    void rotate() {
        if (size() <= 1) return;
        
        T topElement = top();
        pop();
        data.insert(data.begin(), topElement);
    }
    
    // Reverse rotate (move bottom element to top)
    void reverseRotate() {
        if (size() <= 1) return;
        
        T bottomElement = data[0];
        data.erase(data.begin());
        push(bottomElement);
    }
};

// Specialized stack for undo/redo operations
template<typename T>
class UndoRedoStack {
private:
    Stack<T> undoStack;
    Stack<T> redoStack;
    
public:
    void execute(const T& operation) {
        undoStack.push(operation);
        redoStack.clear(); // Clear redo stack when new operation is executed
    }
    
    bool canUndo() const {
        return !undoStack.empty();
    }
    
    bool canRedo() const {
        return !redoStack.empty();
    }
    
    T undo() {
        if (!canUndo()) {
            throw std::runtime_error("Nothing to undo");
        }
        
        T operation = undoStack.top();
        undoStack.pop();
        redoStack.push(operation);
        return operation;
    }
    
    T redo() {
        if (!canRedo()) {
            throw std::runtime_error("Nothing to redo");
        }
        
        T operation = redoStack.top();
        redoStack.pop();
        undoStack.push(operation);
        return operation;
    }
    
    void clear() {
        undoStack.clear();
        redoStack.clear();
    }
    
    size_t undoSize() const {
        return undoStack.size();
    }
    
    size_t redoSize() const {
        return redoStack.size();
    }
    
    void displayStatus() const {
        std::cout << "Undo operations available: " << undoSize() << std::endl;
        std::cout << "Redo operations available: " << redoSize() << std::endl;
    }
};

#endif // STACK_H