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

// Navigation History for mobile banking app-like navigation
// Works like browser history: back goes to previous page, forward goes to next page
template<typename T>
class NavigationHistory {
private:
    vector<T> history;        // Full navigation history
    int currentIndex;         // Current position in history
    
public:
    NavigationHistory() : currentIndex(-1) {}
    
    // Navigate to a new page (like clicking a link)
    void navigateTo(const T& location) {
        // Remove any forward history (if we went back and now navigating forward)
        if (currentIndex < static_cast<int>(history.size()) - 1) {
            history.erase(history.begin() + currentIndex + 1, history.end());
        }
        
        // Add new location to history
        history.push_back(location);
        currentIndex = history.size() - 1;
    }
    
    // Go back to previous page
    bool goBack() {
        if (canGoBack()) {
            currentIndex--;
            return true;
        }
        return false;
    }
    
    // Go forward to next page
    bool goForward() {
        if (canGoForward()) {
            currentIndex++;
            return true;
        }
        return false;
    }
    
    // Check if we can go back
    bool canGoBack() const {
        return currentIndex > 0;
    }
    
    // Check if we can go forward
    bool canGoForward() const {
        return currentIndex >= 0 && currentIndex < static_cast<int>(history.size()) - 1;
    }
    
    // Get current location
    T getCurrent() const {
        if (currentIndex >= 0 && currentIndex < static_cast<int>(history.size())) {
            return history[currentIndex];
        }
        throw runtime_error("No current location");
    }
    
    // Get previous location (without navigating)
    T getPrevious() const {
        if (canGoBack()) {
            return history[currentIndex - 1];
        }
        throw runtime_error("No previous location");
    }
    
    // Get next location (without navigating)
    T getNext() const {
        if (canGoForward()) {
            return history[currentIndex + 1];
        }
        throw runtime_error("No next location");
    }
    
    // Clear all history
    void clear() {
        history.clear();
        currentIndex = -1;
    }
    
    // Get history size
    size_t size() const {
        return history.size();
    }
    
    // Get current index
    int getCurrentIndex() const {
        return currentIndex;
    }
};

#endif // STACK_H