#ifndef SORTING_H
#define SORTING_H

#include <vector>
#include <algorithm>
#include <functional>

template<typename T>
class SortingAlgorithms {
public:
    // Merge Sort with custom comparator (ONLY USED ALGORITHM)
    template<typename Compare>
    static void mergeSortCustom(vector<T>& arr, int left, int right, Compare comp) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSortCustom(arr, left, mid, comp);
            mergeSortCustom(arr, mid + 1, right, comp);
            mergeCustom(arr, left, mid, right, comp);
        }
    }

private:
    // Merge function for custom comparator
    template<typename Compare>
    static void mergeCustom(vector<T>& arr, int left, int mid, int right, Compare comp) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        
        vector<T> leftArr(n1), rightArr(n2);
        
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
};

#endif // SORTING_H