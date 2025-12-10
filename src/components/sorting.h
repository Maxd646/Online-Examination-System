#ifndef SORTING_H
#define SORTING_H

#include <vector>
#include <algorithm>
#include <random>
#include <functional>

template<typename T>
class SortingAlgorithms {
public:
    // Quick Sort with random pivot selection
    static void quickSort(std::vector<T>& arr, int low, int high) {
        if (low < high) {
            int pi = randomizedPartition(arr, low, high);
            quickSort(arr, low, pi - 1);
            quickSort(arr, pi + 1, high);
        }
    }
    
    // Merge Sort
    static void mergeSort(std::vector<T>& arr, int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSort(arr, left, mid);
            mergeSort(arr, mid + 1, right);
            merge(arr, left, mid, right);
        }
    }
    
    // Heap Sort
    static void heapSort(std::vector<T>& arr) {
        int n = arr.size();
        
        // Build max heap
        for (int i = n / 2 - 1; i >= 0; i--) {
            heapify(arr, n, i);
        }
        
        // Extract elements from heap one by one
        for (int i = n - 1; i > 0; i--) {
            std::swap(arr[0], arr[i]);
            heapify(arr, i, 0);
        }
    }
    
    // Insertion Sort (good for small arrays)
    static void insertionSort(std::vector<T>& arr) {
        int n = arr.size();
        for (int i = 1; i < n; i++) {
            T key = arr[i];
            int j = i - 1;
            
            while (j >= 0 && arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }
    
    // Selection Sort
    static void selectionSort(std::vector<T>& arr) {
        int n = arr.size();
        for (int i = 0; i < n - 1; i++) {
            int minIdx = i;
            for (int j = i + 1; j < n; j++) {
                if (arr[j] < arr[minIdx]) {
                    minIdx = j;
                }
            }
            if (minIdx != i) {
                std::swap(arr[i], arr[minIdx]);
            }
        }
    }
    
    // Bubble Sort
    static void bubbleSort(std::vector<T>& arr) {
        int n = arr.size();
        bool swapped;
        
        for (int i = 0; i < n - 1; i++) {
            swapped = false;
            for (int j = 0; j < n - i - 1; j++) {
                if (arr[j] > arr[j + 1]) {
                    std::swap(arr[j], arr[j + 1]);
                    swapped = true;
                }
            }
            if (!swapped) break; // Array is already sorted
        }
    }
    
    // Counting Sort (for integers in a specific range)
    static void countingSort(std::vector<int>& arr, int maxVal) {
        std::vector<int> count(maxVal + 1, 0);
        std::vector<int> output(arr.size());
        
        // Count occurrences
        for (int num : arr) {
            count[num]++;
        }
        
        // Cumulative count
        for (int i = 1; i <= maxVal; i++) {
            count[i] += count[i - 1];
        }
        
        // Build output array
        for (int i = arr.size() - 1; i >= 0; i--) {
            output[count[arr[i]] - 1] = arr[i];
            count[arr[i]]--;
        }
        
        // Copy back to original array
        for (size_t i = 0; i < arr.size(); i++) {
            arr[i] = output[i];
        }
    }
    
    // Radix Sort (for non-negative integers)
    static void radixSort(std::vector<int>& arr) {
        if (arr.empty()) return;
        
        int maxVal = *std::max_element(arr.begin(), arr.end());
        
        for (int exp = 1; maxVal / exp > 0; exp *= 10) {
            countingSortByDigit(arr, exp);
        }
    }
    
    // Bucket Sort (for floating point numbers in range [0, 1))
    static void bucketSort(std::vector<double>& arr) {
        if (arr.empty()) return;
        
        int n = arr.size();
        std::vector<std::vector<double>> buckets(n);
        
        // Put array elements in different buckets
        for (double num : arr) {
            int bucketIndex = static_cast<int>(n * num);
            if (bucketIndex >= n) bucketIndex = n - 1;
            buckets[bucketIndex].push_back(num);
        }
        
        // Sort individual buckets
        for (auto& bucket : buckets) {
            std::sort(bucket.begin(), bucket.end());
        }
        
        // Concatenate all buckets
        int index = 0;
        for (const auto& bucket : buckets) {
            for (double num : bucket) {
                arr[index++] = num;
            }
        }
    }
    
    // Shell Sort
    static void shellSort(std::vector<T>& arr) {
        int n = arr.size();
        
        // Start with a big gap, then reduce the gap
        for (int gap = n / 2; gap > 0; gap /= 2) {
            for (int i = gap; i < n; i++) {
                T temp = arr[i];
                int j;
                
                for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                    arr[j] = arr[j - gap];
                }
                
                arr[j] = temp;
            }
        }
    }
    
    // Tim Sort (hybrid stable sorting algorithm)
    static void timSort(std::vector<T>& arr) {
        // Use std::stable_sort which is typically implemented as Tim Sort
        std::stable_sort(arr.begin(), arr.end());
    }
    
    // Custom comparator versions
    template<typename Compare>
    static void quickSortCustom(std::vector<T>& arr, int low, int high, Compare comp) {
        if (low < high) {
            int pi = partitionCustom(arr, low, high, comp);
            quickSortCustom(arr, low, pi - 1, comp);
            quickSortCustom(arr, pi + 1, high, comp);
        }
    }
    
    template<typename Compare>
    static void mergeSortCustom(std::vector<T>& arr, int left, int right, Compare comp) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSortCustom(arr, left, mid, comp);
            mergeSortCustom(arr, mid + 1, right, comp);
            mergeCustom(arr, left, mid, right, comp);
        }
    }
    
    // Utility functions for performance analysis
    static void measureSortingTime(std::vector<T> arr, const std::string& algorithmName, 
                                  std::function<void(std::vector<T>&)> sortFunction) {
        auto start = std::chrono::high_resolution_clock::now();
        sortFunction(arr);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << algorithmName << " took " << duration.count() << " microseconds" << std::endl;
    }
    
    // Check if array is sorted
    static bool isSorted(const std::vector<T>& arr) {
        for (size_t i = 1; i < arr.size(); i++) {
            if (arr[i] < arr[i - 1]) {
                return false;
            }
        }
        return true;
    }
    
    // Generate test data
    static std::vector<int> generateRandomArray(int size, int minVal = 0, int maxVal = 1000) {
        std::vector<int> arr(size);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(minVal, maxVal);
        
        for (int& num : arr) {
            num = dis(gen);
        }
        
        return arr;
    }
    
    static std::vector<int> generateSortedArray(int size) {
        std::vector<int> arr(size);
        for (int i = 0; i < size; i++) {
            arr[i] = i;
        }
        return arr;
    }
    
    static std::vector<int> generateReverseSortedArray(int size) {
        std::vector<int> arr(size);
        for (int i = 0; i < size; i++) {
            arr[i] = size - i - 1;
        }
        return arr;
    }

private:
    static int randomizedPartition(std::vector<T>& arr, int low, int high) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(low, high);
        int randomIndex = dis(gen);
        
        std::swap(arr[randomIndex], arr[high]);
        return partition(arr, low, high);
    }
    
    static int partition(std::vector<T>& arr, int low, int high) {
        T pivot = arr[high];
        int i = low - 1;
        
        for (int j = low; j <= high - 1; j++) {
            if (arr[j] < pivot) {
                i++;
                std::swap(arr[i], arr[j]);
            }
        }
        std::swap(arr[i + 1], arr[high]);
        return i + 1;
    }
    
    template<typename Compare>
    static int partitionCustom(std::vector<T>& arr, int low, int high, Compare comp) {
        T pivot = arr[high];
        int i = low - 1;
        
        for (int j = low; j <= high - 1; j++) {
            if (comp(arr[j], pivot)) {
                i++;
                std::swap(arr[i], arr[j]);
            }
        }
        std::swap(arr[i + 1], arr[high]);
        return i + 1;
    }
    
    static void merge(std::vector<T>& arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        
        std::vector<T> leftArr(n1), rightArr(n2);
        
        for (int i = 0; i < n1; i++)
            leftArr[i] = arr[left + i];
        for (int j = 0; j < n2; j++)
            rightArr[j] = arr[mid + 1 + j];
        
        int i = 0, j = 0, k = left;
        
        while (i < n1 && j < n2) {
            if (leftArr[i] <= rightArr[j]) {
                arr[k] = leftArr[i];
                i++;
            } else {
                arr[k] = rightArr[j];
                j++;
            }
            k++;
        }
        
        while (i < n1) {
            arr[k] = leftArr[i];
            i++;
            k++;
        }
        
        while (j < n2) {
            arr[k] = rightArr[j];
            j++;
            k++;
        }
    }
    
    template<typename Compare>
    static void mergeCustom(std::vector<T>& arr, int left, int mid, int right, Compare comp) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        
        std::vector<T> leftArr(n1), rightArr(n2);
        
        for (int i = 0; i < n1; i++)
            leftArr[i] = arr[left + i];
        for (int j = 0; j < n2; j++)
            rightArr[j] = arr[mid + 1 + j];
        
        int i = 0, j = 0, k = left;
        
        while (i < n1 && j < n2) {
            if (!comp(rightArr[j], leftArr[i])) {
                arr[k] = leftArr[i];
                i++;
            } else {
                arr[k] = rightArr[j];
                j++;
            }
            k++;
        }
        
        while (i < n1) {
            arr[k] = leftArr[i];
            i++;
            k++;
        }
        
        while (j < n2) {
            arr[k] = rightArr[j];
            j++;
            k++;
        }
    }
    
    static void heapify(std::vector<T>& arr, int n, int i) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        
        if (left < n && arr[left] > arr[largest])
            largest = left;
        
        if (right < n && arr[right] > arr[largest])
            largest = right;
        
        if (largest != i) {
            std::swap(arr[i], arr[largest]);
            heapify(arr, n, largest);
        }
    }
    
    static void countingSortByDigit(std::vector<int>& arr, int exp) {
        std::vector<int> output(arr.size());
        std::vector<int> count(10, 0);
        
        for (int num : arr) {
            count[(num / exp) % 10]++;
        }
        
        for (int i = 1; i < 10; i++) {
            count[i] += count[i - 1];
        }
        
        for (int i = arr.size() - 1; i >= 0; i--) {
            output[count[(arr[i] / exp) % 10] - 1] = arr[i];
            count[(arr[i] / exp) % 10]--;
        }
        
        for (size_t i = 0; i < arr.size(); i++) {
            arr[i] = output[i];
        }
    }
};

#endif // SORTING_H