// A1r — MSD Radix Sort без переключения на QuickSort (Codeforces)

#include <iostream>
#include <string>
#include <vector>

static const int RADIX = 256;

static int charAt(const std::string& s, int d) {
    return (d < static_cast<int>(s.size())) ? static_cast<unsigned char>(s[d]) : -1;
}

static void msdSort(std::vector<std::string>& a, std::vector<std::string>& aux,
                    int lo, int hi, int d) {
    if (hi <= lo) return;

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

static void msdRadixSort(std::vector<std::string>& arr) {
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

    msdRadixSort(arr);

    for (int i = 0; i < n; ++i) {
        if (i) std::cout << '\n';
        std::cout << arr[i];
    }
    return 0;
}
