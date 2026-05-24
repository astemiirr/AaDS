// A1rq — MSD Radix Sort + String QuickSort при размере подмассива < 74 (Codeforces)

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

static const int ALPHABET_SIZE = 74;
static const int RADIX = 256;

static int pivotIndex(int low, int high) {
    return low + (high - low) / 2;
}

static void stringQuickSortHelper(std::vector<std::string>& arr, int low, int high, int depth) {
    if (low >= high) return;
    int mid = pivotIndex(low, high);
    char pivotChar = (depth < static_cast<int>(arr[mid].length())) ? arr[mid][depth] : 0;

    int lt = low, gt = high, i = low;
    while (i <= gt) {
        char c = (depth < static_cast<int>(arr[i].length())) ? arr[i][depth] : 0;
        if (c < pivotChar) std::swap(arr[lt++], arr[i++]);
        else if (c > pivotChar) std::swap(arr[i], arr[gt--]);
        else ++i;
    }

    stringQuickSortHelper(arr, low, lt - 1, depth);
    if (pivotChar != 0) stringQuickSortHelper(arr, lt, gt, depth + 1);
    stringQuickSortHelper(arr, gt + 1, high, depth);
}

static int charAt(const std::string& s, int d) {
    return (d < static_cast<int>(s.size())) ? static_cast<unsigned char>(s[d]) : -1;
}

static void msdSort(std::vector<std::string>& a, std::vector<std::string>& aux,
                    int lo, int hi, int d) {
    if (hi <= lo) return;

    if (hi - lo + 1 < ALPHABET_SIZE) {
        stringQuickSortHelper(a, lo, hi, d);
        return;
    }

    std::vector<int> count(RADIX + 2, 0);
    for (int i = lo; i <= hi; ++i) ++count[charAt(a[i], d) + 2];
    for (int r = 0; r < RADIX + 1; ++r) count[r + 1] += count[r];
    for (int i = lo; i <= hi; ++i) {
        int c = charAt(a[i], d);
        aux[count[c + 1]++] = a[i];
    }
    for (int i = lo; i <= hi; ++i) a[i] = aux[i - lo];

    for (int r = 0; r < RADIX; ++r) {
        msdSort(a, aux, lo + count[r], lo + count[r + 1] - 1, d + 1);
    }
    if (count[RADIX + 1] > count[RADIX]) {
        msdSort(a, aux, lo + count[RADIX], lo + count[RADIX + 1] - 1, d + 1);
    }
}

static void msdRadixSortWithQS(std::vector<std::string>& arr) {
    if (arr.empty()) return;
    std::vector<std::string> aux(arr.size());
    msdSort(arr, aux, 0, static_cast<int>(arr.size()) - 1, 0);
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    if (!(std::cin >> n)) return 0;
    std::vector<std::string> arr(n);
    for (int i = 0; i < n; ++i) std::cin >> arr[i];

    msdRadixSortWithQS(arr);

    for (int i = 0; i < n; ++i) {
        if (i) std::cout << '\n';
        std::cout << arr[i];
    }
    return 0;
}
