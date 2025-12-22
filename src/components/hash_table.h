#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <vector>
#include <functional>
#include <iostream>
#include <algorithm>
#include <stdexcept>

using namespace std;

template<typename K, typename V>
class HashTable {
private:
    struct Entry {
        K key;
        V value;
        bool occupied;
        bool deleted;
        
        Entry() : occupied(false), deleted(false) {}
    };
    
    vector<Entry> table;
    size_t capacity;
    size_t size;
    double maxLoadFactor;
    
    size_t hash(const K& key) const {
        return std::hash<K>{}(key) % capacity;
    }
    
    size_t probe(size_t index) const {
        return (index + 1) % capacity;
    }
    

    
    void resize() {
        vector<Entry> oldTable = table;
        size_t oldCapacity = capacity;
        
        capacity *= 2;
        table = vector<Entry>(capacity);
        size = 0;
        
        for (size_t i = 0; i < oldCapacity; ++i) {
            if (oldTable[i].occupied && !oldTable[i].deleted) {
                insert(oldTable[i].key, oldTable[i].value);
            }
        }
    }
    
public:
    HashTable(size_t initialCapacity = 16, double maxLoad = 0.75) 
        : capacity(initialCapacity), size(0), maxLoadFactor(maxLoad) {
        table.resize(capacity);
    }
    
    void insert(const K& key, const V& value) {
        if (static_cast<double>(size) / capacity >= maxLoadFactor) {
            resize();
        }
        
        size_t index = hash(key);
        size_t originalIndex = index;
        size_t attempt = 0;
        
        do {
            if (!table[index].occupied || table[index].deleted) {
                if (!table[index].occupied || table[index].deleted) {
                    size++;
                }
                table[index].key = key;
                table[index].value = value;
                table[index].occupied = true;
                table[index].deleted = false;
                return;
            }
            
            if (table[index].key == key) {
                // Update existing key
                table[index].value = value;
                return;
            }
            
            // Linear probing
            index = probe(index);
            attempt++;
            
        } while (index != originalIndex && attempt < capacity);
        
        // If we reach here, table is full (shouldn't happen with proper resizing)
        throw runtime_error("Hash table is full");
    }
    
    V* find(const K& key) {
        size_t index = hash(key);
        size_t originalIndex = index;
        size_t attempt = 0;
        
        do {
            if (!table[index].occupied) {
                return nullptr;
            }
            
            if (!table[index].deleted && table[index].key == key) {
                return &table[index].value;
            }
            
            index = probe(index);
            attempt++;
            
        } while (index != originalIndex && attempt < capacity);
        
        return nullptr;
    }
    
    const V* find(const K& key) const {
        size_t index = hash(key);
        size_t originalIndex = index;
        size_t attempt = 0;
        
        do {
            if (!table[index].occupied) {
                return nullptr;
            }
            
            if (!table[index].deleted && table[index].key == key) {
                return &table[index].value;
            }
            
            index = probe(index);
            attempt++;
            
        } while (index != originalIndex && attempt < capacity);
        
        return nullptr;
    }
    
    bool remove(const K& key) {
        size_t index = hash(key);
        size_t originalIndex = index;
        size_t attempt = 0;
        
        do {
            if (!table[index].occupied) {
                return false;
            }
            
            if (!table[index].deleted && table[index].key == key) {
                table[index].deleted = true;
                size--;
                return true;
            }
            
            index = probe(index);
            attempt++;
            
        } while (index != originalIndex && attempt < capacity);
        
        return false;
    }
    
    bool contains(const K& key) const {
        return find(key) != nullptr;
    }
    
    size_t getSize() const { 
        return size; 
    }
    
    bool empty() const { 
        return size == 0; 
    }
    
    double getLoadFactor() const {
        return static_cast<double>(size) / capacity;
    }
    
    size_t getCapacity() const {
        return capacity;
    }
    
    void clear() {
        table = vector<Entry>(capacity);
        size = 0;
    }
    
    // Get all values for cleanup purposes
    vector<V> getAllValues() const {
        vector<V> values;
        for (const auto& entry : table) {
            if (entry.occupied && !entry.deleted) {
                values.push_back(entry.value);
            }
        }
        return values;
    }
};

#endif // HASH_TABLE_H