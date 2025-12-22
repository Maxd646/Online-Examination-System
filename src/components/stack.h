#ifndef STACK_H
#define STACK_H

#include <vector>
#include <stdexcept>

using namespace std;

template<typename T>
class Stack {
private:
    vector<T> data;
    
public:
    Stack() {}
    
    void push(const T& value) {
        data.push_back(value);
    }
    
    void pop() {
        if (empty()) {
            throw runtime_error("Stack is empty");
        }
        data.pop_back();
    }
    
    T& top() {
        if (empty()) {
            throw runtime_error("Stack is empty");
        }
        return data.back();
    }
    
    const T& top() const {
        if (empty()) {
            throw runtime_error("Stack is empty");
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
            throw runtime_error("Nothing to undo");
        }
        
        T operation = undoStack.top();
        undoStack.pop();
        redoStack.push(operation);
        return operation;
    }
    
    T redo() {
        if (!canRedo()) {
            throw runtime_error("Nothing to redo");
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
};

#endif // STACK_H