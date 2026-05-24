#include "sort_metrics.h"

size_t SortMetrics::charComparisons = 0;

void SortMetrics::resetComparisons() {
    charComparisons = 0;
}

size_t SortMetrics::getComparisons() {
    return charComparisons;
}

bool SortMetrics::compareChars(char a, char b) {
    ++charComparisons;
    return a < b;
}

int SortMetrics::compareStrings(const std::string& a, const std::string& b) {
    size_t minLen = std::min(a.length(), b.length());
    for (size_t i = 0; i < minLen; ++i) {
        ++charComparisons;
        if (a[i] != b[i]) {
            return a[i] < b[i] ? -1 : 1;
        }
    }
    if (a.length() != b.length()) {
        ++charComparisons;
        return a.length() < b.length() ? -1 : 1;
    }
    return 0;
}
