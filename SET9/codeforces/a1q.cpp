// A1q — тернарный String QuickSort (Codeforces)

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

static int pivotIndex(int low, int high) {
    return low + (high - low) / 2;
}

void stringQuickSortHelper(std::vector<std::string>& arr, int low, int high, int depth) {
    if (low >= high) return;

    int mid = pivotIndex(low, high);
    char pivotChar = (depth < static_cast<int>(arr[mid].length())) ? arr[mid][depth] : 0;

    int lt = low, gt = high, i = low;
    while (i <= gt) {
        char currChar = (depth < static_cast<int>(arr[i].length())) ? arr[i][depth] : 0;
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

void stringQuickSort(std::vector<std::string>& arr) {
    if (!arr.empty()) {
        stringQuickSortHelper(arr, 0, static_cast<int>(arr.size()) - 1, 0);
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    if (!(std::cin >> n)) return 0;

    std::vector<std::string> arr(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> arr[i];
    }

    stringQuickSort(arr);

    for (int i = 0; i < n; ++i) {
        if (i) std::cout << '\n';
        std::cout << arr[i];
    }
    return 0;
}
