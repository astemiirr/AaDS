#ifndef ADAPTED_SORTS_H
#define ADAPTED_SORTS_H

#include <vector>
#include <string>
#include <utility>
#include "sort_metrics.h"
#include "sort_common.h"

// --- Тернарный String QuickSort (опорный элемент — середина подмассива) ---

inline void stringQuickSortHelper(std::vector<std::string>& arr, int low, int high, int depth) {
    if (low >= high) return;

    int mid = pivotIndex(low, high);
    char pivotChar = (depth < static_cast<int>(arr[mid].length())) ? arr[mid][depth] : 0;

    int lt = low, gt = high;
    int i = low;

    while (i <= gt) {
        char currChar = (depth < static_cast<int>(arr[i].length())) ? arr[i][depth] : 0;
        SortMetrics::compareChars(currChar, pivotChar);

        if (currChar < pivotChar) {
            std::swap(arr[lt++], arr[i++]);
        } else if (currChar > pivotChar) {
            std::swap(arr[i], arr[gt--]);
        } else {
            ++i;
        }
    }

    stringQuickSortHelper(arr, low, lt - 1, depth);
    if (pivotChar != 0) {
        stringQuickSortHelper(arr, lt, gt, depth + 1);
    }
    stringQuickSortHelper(arr, gt + 1, high, depth);
}

inline void string_quick_sort(std::vector<std::string>& arr) {
    if (!arr.empty()) {
        stringQuickSortHelper(arr, 0, static_cast<int>(arr.size()) - 1, 0);
    }
}

// --- String MergeSort с LCP (алгоритм StringMerge из конспекта) ---

inline std::pair<int, int> lcpCompare(const std::string& A, const std::string& B, int k) {
    int lenA = static_cast<int>(A.length());
    int lenB = static_cast<int>(B.length());

    int i = k;
    while (i < lenA && i < lenB) {
        SortMetrics::compareChars(A[i], B[i]);
        if (A[i] != B[i]) break;
        ++i;
    }

    int lcp = i;
    int x;

    if (lcp == lenA && lcp == lenB) x = 0;
    else if (lcp == lenA) x = -1;
    else if (lcp == lenB) x = 1;
    else x = (A[lcp] < B[lcp]) ? -1 : 1;

    return {x, lcp};
}

struct StringWithLCP {
    std::string str;
    int lcp;
    StringWithLCP() : lcp(0) {}
    StringWithLCP(const std::string& s, int l) : str(s), lcp(l) {}
};

inline std::vector<StringWithLCP> stringMerge(
    const std::vector<StringWithLCP>& P,
    const std::vector<StringWithLCP>& Q) {

    std::vector<StringWithLCP> R;
    int i = 0, j = 0;
    int m = static_cast<int>(P.size());
    int f = static_cast<int>(Q.size());

    // Инвариант StringMerge: ki = LCP(X, Si), pj = LCP(X, Wj), X — последняя строка в R
    int ki = 0;
    int pj = 0;

    auto refreshKi = [&]() {
        if (i < m) {
            auto [x, h] = lcpCompare(P[i].str, R.back().str, 0);
            (void)x;
            ki = h;
        }
    };
    auto refreshPj = [&]() {
        if (j < f) {
            auto [x, h] = lcpCompare(Q[j].str, R.back().str, 0);
            (void)x;
            pj = h;
        }
    };

    while (i < m && j < f) {
        if (ki > pj) {
            R.push_back(StringWithLCP(P[i].str, ki));
            ++i;
            refreshKi();
        } else if (ki < pj) {
            R.push_back(StringWithLCP(Q[j].str, pj));
            ++j;
            refreshPj();
        } else {
            auto [x, h] = lcpCompare(P[i].str, Q[j].str, ki);

            if (x < 0) {
                R.push_back(StringWithLCP(P[i].str, ki));
                ++i;
                pj = h;
                refreshKi();
            } else {
                R.push_back(StringWithLCP(Q[j].str, pj));
                ++j;
                ki = h;
                refreshPj();
            }
        }
    }

    while (i < m) {
        R.push_back(StringWithLCP(P[i].str, ki));
        ++i;
        if (i < m) refreshKi();
    }
    while (j < f) {
        R.push_back(StringWithLCP(Q[j].str, pj));
        ++j;
        if (j < f) refreshPj();
    }

    return R;
}

inline std::vector<StringWithLCP> stringMergeSortHelper(
    const std::vector<std::string>& arr, int left, int right) {

    if (left == right) {
        return {StringWithLCP(arr[left], 0)};
    }

    int mid = left + (right - left) / 2;
    auto P = stringMergeSortHelper(arr, left, mid);
    auto Q = stringMergeSortHelper(arr, mid + 1, right);

    return stringMerge(P, Q);
}

inline void string_merge_sort(std::vector<std::string>& arr) {
    if (arr.empty()) return;
    auto result = stringMergeSortHelper(arr, 0, static_cast<int>(arr.size()) - 1);
    for (size_t k = 0; k < arr.size(); ++k) {
        arr[k] = result[k].str;
    }
}

// --- MSD Radix Sort (байтовый алфавит 256, по Sedgewick algs4) ---

constexpr int MSD_RADIX = 256;

inline int msdCharAt(const std::string& s, int depth) {
    return (depth < static_cast<int>(s.length()))
        ? static_cast<unsigned char>(s[depth])
        : -1;
}

inline void msdRadixSortHelper(std::vector<std::string>& arr,
                               std::vector<std::string>& aux,
                               int low, int high, int depth) {
    if (high <= low) return;

    std::vector<int> count(MSD_RADIX + 2, 0);

    for (int i = low; i <= high; ++i) {
        ++count[msdCharAt(arr[i], depth) + 2];
    }

    for (int r = 0; r < MSD_RADIX + 1; ++r) {
        count[r + 1] += count[r];
    }

    for (int i = low; i <= high; ++i) {
        int c = msdCharAt(arr[i], depth);
        aux[count[c + 1]++] = arr[i];
    }

    for (int i = low; i <= high; ++i) {
        arr[i] = aux[i - low];
    }

    for (int r = 0; r < MSD_RADIX; ++r) {
        msdRadixSortHelper(arr, aux,
                           low + count[r],
                           low + count[r + 1] - 1,
                           depth + 1);
    }
    if (count[MSD_RADIX + 1] > count[MSD_RADIX]) {
        msdRadixSortHelper(arr, aux,
                           low + count[MSD_RADIX],
                           low + count[MSD_RADIX + 1] - 1,
                           depth + 1);
    }
}

inline void msd_radix_sort(std::vector<std::string>& arr) {
    if (arr.empty()) return;
    std::vector<std::string> aux(arr.size());
    msdRadixSortHelper(arr, aux, 0, static_cast<int>(arr.size()) - 1, 0);
}

inline void msdRadixSortWithQSHelper(std::vector<std::string>& arr,
                                     std::vector<std::string>& aux,
                                     int low, int high, int depth) {
    if (high <= low) return;

    if (high - low + 1 < MSD_QS_CUTOFF) {
        stringQuickSortHelper(arr, low, high, depth);
        return;
    }

    std::vector<int> count(MSD_RADIX + 2, 0);

    for (int i = low; i <= high; ++i) {
        ++count[msdCharAt(arr[i], depth) + 2];
    }

    for (int r = 0; r < MSD_RADIX + 1; ++r) {
        count[r + 1] += count[r];
    }

    for (int i = low; i <= high; ++i) {
        int c = msdCharAt(arr[i], depth);
        aux[count[c + 1]++] = arr[i];
    }

    for (int i = low; i <= high; ++i) {
        arr[i] = aux[i - low];
    }

    for (int r = 0; r < MSD_RADIX; ++r) {
        msdRadixSortWithQSHelper(arr, aux,
                                 low + count[r],
                                 low + count[r + 1] - 1,
                                 depth + 1);
    }
    if (count[MSD_RADIX + 1] > count[MSD_RADIX]) {
        msdRadixSortWithQSHelper(arr, aux,
                                 low + count[MSD_RADIX],
                                 low + count[MSD_RADIX + 1] - 1,
                                 depth + 1);
    }
}

inline void msd_radix_sort_with_qs(std::vector<std::string>& arr) {
    if (arr.empty()) return;
    std::vector<std::string> aux(arr.size());
    msdRadixSortWithQSHelper(arr, aux, 0, static_cast<int>(arr.size()) - 1, 0);
}

#endif
