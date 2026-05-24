#include "string_sort_tester.h"

#include <algorithm>
#include <numeric>

StringSortTester::StringSortTester(int iterations)
    : iterationsPerTest(iterations) {}

void StringSortTester::test_algorithm(
    const std::string& algoName,
    std::function<void(std::vector<std::string>&)> sortFunc,
    const std::vector<std::string>& testArray,
    const std::string& arrayType) {

    std::vector<double> times;
    std::vector<size_t> comparisons;

    for (int run = 0; run < iterationsPerTest; ++run) {
        auto arr = testArray;

        SortMetrics::resetComparisons();
        auto start = std::chrono::high_resolution_clock::now();
        sortFunc(arr);
        auto end = std::chrono::high_resolution_clock::now();

        double timeUs = std::chrono::duration<double, std::micro>(end - start).count();
        times.push_back(timeUs);
        comparisons.push_back(SortMetrics::getComparisons());

        if (run == 0 && !std::is_sorted(arr.begin(), arr.end())) {
            std::cerr << "WARNING: " << algoName
                      << " failed on " << arrayType
                      << " array (size " << testArray.size() << ")!\n";
        }
    }

    double avgTime = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    size_t avgComparisons = std::accumulate(comparisons.begin(), comparisons.end(), 0ULL)
                            / comparisons.size();

    results.push_back({
        algoName,
        arrayType,
        static_cast<int>(testArray.size()),
        avgTime,
        avgComparisons,
        iterationsPerTest
    });
}

void StringSortTester::export_results_csv(const std::string& filename) {
    std::ofstream file(filename);
    file << "Algorithm,ArrayType,Size,AvgTime(us),AvgComparisons,Iterations\n";

    for (const auto& r : results) {
        file << r.algorithmName << ","
             << r.arrayType << ","
             << r.arraySize << ","
             << r.avgTime << ","
             << r.avgComparisons << ","
             << r.iterations << "\n";
    }
}

void StringSortTester::print_results() {
    std::cout << "\n====== TEST RESULTS ======\n";
    for (const auto& r : results) {
        std::cout << r.algorithmName
                  << " | " << r.arrayType
                  << " | Size: " << r.arraySize
                  << " | Time: " << r.avgTime << " us"
                  << " | Comparisons: " << r.avgComparisons
                  << " | Iterations: " << r.iterations << "\n";
    }
}
