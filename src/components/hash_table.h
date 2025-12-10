#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <vector>
#include <functional>
#include <iostream>

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
    
    std::vector<Entry> table;
    size_t capacity;
    size_t size;
    double maxLoadFactor;
    
    size_t hash(const K& key) const {
        return std::hash<K>{}(key) % capacity;
    }
    
    size_t probe(size_t index) const {
        return (index + 1) % capacity;
    }
    
    size_t quadraticProbe(size_t index, size_t attempt) const {
        return (index + attempt * attempt) % capacity;
    }
    
    void resize() {
        std::vector<Entry> oldTable = table;
        size_t oldCapacity = capacity;
        
        capacity *= 2;
        table = std::vector<Entry>(capacity);
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
        throw std::runtime_error("Hash table is full");
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
        table = std::vector<Entry>(capacity);
        size = 0;
    }
    
    // Get all keys
    std::vector<K> getKeys() const {
        std::vector<K> keys;
        for (const auto& entry : table) {
            if (entry.occupied && !entry.deleted) {
                keys.push_back(entry.key);
            }
        }
        return keys;
    }
    
    // Get all values
    std::vector<V> getValues() const {
        std::vector<V> values;
        for (const auto& entry : table) {
            if (entry.occupied && !entry.deleted) {
                values.push_back(entry.value);
            }
        }
        return values;
    }
    
    // Get all key-value pairs
    std::vector<std::pair<K, V>> getEntries() const {
        std::vector<std::pair<K, V>> entries;
        for (const auto& entry : table) {
            if (entry.occupied && !entry.deleted) {
                entries.push_back({entry.key, entry.value});
            }
        }
        return entries;
    }
    
    void display() const {
        std::cout << "Hash Table (size: " << size << ", capacity: " << capacity 
                  << ", load factor: " << getLoadFactor() << "):" << std::endl;
        
        for (size_t i = 0; i < capacity; ++i) {
            std::cout << "[" << i << "] ";
            if (table[i].occupied && !table[i].deleted) {
                std::cout << table[i].key << " -> " << table[i].value;
            } else if (table[i].deleted) {
                std::cout << "(deleted)";
            } else {
                std::cout << "(empty)";
            }
            std::cout << std::endl;
        }
    }
    
    // Statistics
    void printStatistics() const {
        std::cout << "Hash Table Statistics:" << std::endl;
        std::cout << "Size: " << size << std::endl;
        std::cout << "Capacity: " << capacity << std::endl;
        std::cout << "Load Factor: " << getLoadFactor() << std::endl;
        std::cout << "Max Load Factor: " << maxLoadFactor << std::endl;
        
        // Count collisions
        size_t collisions = 0;
        for (size_t i = 0; i < capacity; ++i) {
            if (table[i].occupied && !table[i].deleted) {
                if (hash(table[i].key) != i) {
                    collisions++;
                }
            }
        }
        std::cout << "Collisions: " << collisions << std::endl;
    }
};

// Specialized hash table for string keys with case-insensitive comparison
class CaseInsensitiveHashTable {
private:
    HashTable<std::string, std::string> table;
    
    std::string toLower(const std::string& str) const {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
public:
    CaseInsensitiveHashTable(size_t initialCapacity = 16) : table(initialCapacity) {}
    
    void insert(const std::string& key, const std::string& value) {
        table.insert(toLower(key), value);
    }
    
    std::string* find(const std::string& key) {
        return table.find(toLower(key));
    }
    
    bool remove(const std::string& key) {
        return table.remove(toLower(key));
    }
    
    bool contains(const std::string& key) const {
        return table.contains(toLower(key));
    }
    
    size_t getSize() const { return table.getSize(); }
    bool empty() const { return table.empty(); }
    void clear() { table.clear(); }
    
    void display() const {
        std::cout << "Case-Insensitive ";
        table.display();
    }
};

#endif // HASH_TABLE_H