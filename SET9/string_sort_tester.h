#ifndef STRING_SORT_TESTER_H
#define STRING_SORT_TESTER_H

#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "sort_metrics.h"

class StringSortTester {
private:
    struct TestResult {
        std::string algorithmName;
        std::string arrayType;
        int arraySize;
        double avgTime;
        size_t avgComparisons;
        int iterations;
    };

    std::vector<TestResult> results;
    int iterationsPerTest;

public:
    explicit StringSortTester(int iterations = 5);

    void test_algorithm(const std::string& algoName,
                        std::function<void(std::vector<std::string>&)> sortFunc,
                        const std::vector<std::string>& testArray,
                        const std::string& arrayType);

    void export_results_csv(const std::string& filename);
    void print_results();
};

#endif
