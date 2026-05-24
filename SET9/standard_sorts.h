#ifndef STANDARD_SORTS_H
#define STANDARD_SORTS_H

#include <vector>
#include <string>
#include <utility>
#include "sort_metrics.h"
#include "sort_common.h"

inline int partition(std::vector<std::string>& arr, int low, int high) {
    int mid = pivotIndex(low, high);
    std::swap(arr[mid], arr[high]);
    const std::string& pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        if (SortMetrics::compareStrings(arr[j], pivot) < 0) {
            ++i;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

inline void quickSortHelper(std::vector<std::string>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSortHelper(arr, low, pi - 1);
        quickSortHelper(arr, pi + 1, high);
    }
}

inline void standard_quick_sort(std::vector<std::string>& arr) {
    if (!arr.empty()) {
        quickSortHelper(arr, 0, static_cast<int>(arr.size()) - 1);
    }
}

inline void merge(std::vector<std::string>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<std::string> L(n1), R(n2);
    for (int i = 0; i < n1; ++i) L[i] = arr[left + i];
    for (int i = 0; i < n2; ++i) R[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (SortMetrics::compareStrings(L[i], R[j]) <= 0) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

inline void mergeSortHelper(std::vector<std::string>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortHelper(arr, left, mid);
        mergeSortHelper(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

inline void standard_merge_sort(std::vector<std::string>& arr) {
    if (!arr.empty()) {
        mergeSortHelper(arr, 0, static_cast<int>(arr.size()) - 1);
    }
}

#endif
