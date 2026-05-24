// A1m — String MergeSort с LCP (Codeforces)

#include <iostream>
#include <string>
#include <utility>
#include <vector>

static std::pair<int, int> lcpCompare(const std::string& A, const std::string& B, int k) {
    int lenA = static_cast<int>(A.size());
    int lenB = static_cast<int>(B.size());
    int i = k;
    while (i < lenA && i < lenB) {
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

struct Node {
    std::string str;
    int lcp;
};

static std::vector<Node> stringMerge(const std::vector<Node>& P, const std::vector<Node>& Q) {
    std::vector<Node> R;
    int i = 0, j = 0;
    int m = static_cast<int>(P.size());
    int f = static_cast<int>(Q.size());
    int ki = 0, pj = 0;

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
            R.push_back({P[i].str, ki});
            ++i;
            refreshKi();
        } else if (ki < pj) {
            R.push_back({Q[j].str, pj});
            ++j;
            refreshPj();
        } else {
            auto [x, h] = lcpCompare(P[i].str, Q[j].str, ki);
            if (x < 0) {
                R.push_back({P[i].str, ki});
                ++i;
                pj = h;
                refreshKi();
            } else {
                R.push_back({Q[j].str, pj});
                ++j;
                ki = h;
                refreshPj();
            }
        }
    }
    while (i < m) {
        R.push_back({P[i].str, ki});
        ++i;
        if (i < m) refreshKi();
    }
    while (j < f) {
        R.push_back({Q[j].str, pj});
        ++j;
        if (j < f) refreshPj();
    }
    return R;
}

static std::vector<Node> mergeSort(const std::vector<std::string>& arr, int l, int r) {
    if (l == r) return {{arr[l], 0}};
    int mid = l + (r - l) / 2;
    auto P = mergeSort(arr, l, mid);
    auto Q = mergeSort(arr, mid + 1, r);
    return stringMerge(P, Q);
}

static void stringMergeSort(std::vector<std::string>& arr) {
    if (arr.empty()) return;
    auto sorted = mergeSort(arr, 0, static_cast<int>(arr.size()) - 1);
    for (size_t i = 0; i < arr.size(); ++i) arr[i] = sorted[i].str;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    if (!(std::cin >> n)) return 0;
    std::vector<std::string> arr(n);
    for (int i = 0; i < n; ++i) std::cin >> arr[i];

    stringMergeSort(arr);

    for (int i = 0; i < n; ++i) {
        if (i) std::cout << '\n';
        std::cout << arr[i];
    }
    return 0;
}
