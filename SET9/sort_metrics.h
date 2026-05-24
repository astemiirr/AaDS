#ifndef SORT_METRICS_H
#define SORT_METRICS_H

#include <algorithm>
#include <cstddef>
#include <string>

// Счётчик посимвольных сравнений для всех алгоритмов сортировки
class SortMetrics {
public:
    static size_t charComparisons;

    static void resetComparisons();
    static size_t getComparisons();

    // Одно посимвольное сравнение (учитывается в статистике)
    static bool compareChars(char a, char b);

    // Полное лексикографическое сравнение строк
    static int compareStrings(const std::string& a, const std::string& b);
};

#endif
