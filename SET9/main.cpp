#include <iostream>
#include <iomanip>

#include "string_generator.h"
#include "string_sort_tester.h"
#include "standard_sorts.h"
#include "adapted_sorts.h"

using namespace std;

int main() {
    // Формат вывода
    cout << fixed << setprecision(2);
    
    // Инициализируем генератор и тестер
    StringGenerator generator(42);
    StringSortTester tester(5);  // 5 запусков для усреднения
    
    // Генерируем максимальные массивы для каждого типа
    const int MAX_LEN = 3000;
    const int STEP = 100;
    
    cout << "Generating test arrays...\n";
    
    auto random_vec = generator.generateTypedArray(MAX_LEN, StringGenerator::RANDOM);
    auto reverse_vec = generator.generateTypedArray(MAX_LEN, StringGenerator::REVERSE_SORTED);
    auto nearly_sorted_vec = generator.generateTypedArray(MAX_LEN, StringGenerator::NEARLY_SORTED);
    auto prefix_vec = generator.generateWithCommonPrefix(MAX_LEN, 50);
    
    cout << "Arrays generated. Starting tests...\n\n";
    
    // Тестируем для разных размеров
    for (int curr_len = 100; curr_len <= MAX_LEN; curr_len += STEP) {
        cout << "Testing size: " << curr_len << "...\n";
        
        // Получаем подмассивы
        auto random_subvec = generator.getSubarray(random_vec, curr_len);
        auto reverse_subvec = generator.getSubarray(reverse_vec, curr_len);
        auto nearly_subvec = generator.getSubarray(nearly_sorted_vec, curr_len);
        auto prefix_subvec = generator.getSubarray(prefix_vec, curr_len);
        
        // Тестируем стандартные алгоритмы
        tester.test_algorithm("QuickSort", standard_quick_sort, random_subvec, "Random");
        tester.test_algorithm("MergeSort", standard_merge_sort, random_subvec, "Random");
        
        tester.test_algorithm("QuickSort", standard_quick_sort, reverse_subvec, "Reverse");
        tester.test_algorithm("MergeSort", standard_merge_sort, reverse_subvec, "Reverse");
        
        tester.test_algorithm("QuickSort", standard_quick_sort, nearly_subvec, "NearlySorted");
        tester.test_algorithm("MergeSort", standard_merge_sort, nearly_subvec, "NearlySorted");
        
        tester.test_algorithm("QuickSort", standard_quick_sort, prefix_subvec, "Prefix");
        tester.test_algorithm("MergeSort", standard_merge_sort, prefix_subvec, "Prefix");
        
        // Тестируем адаптированные алгоритмы
        tester.test_algorithm("StringQuickSort", string_quick_sort, random_subvec, "Random");
        tester.test_algorithm("StringMergeSort", string_merge_sort, random_subvec, "Random");
        tester.test_algorithm("MSDRadixSort", msd_radix_sort, random_subvec, "Random");
        tester.test_algorithm("MSDRadixSort+QS", msd_radix_sort_with_qs, random_subvec, "Random");
        
        tester.test_algorithm("StringQuickSort", string_quick_sort, reverse_subvec, "Reverse");
        tester.test_algorithm("StringMergeSort", string_merge_sort, reverse_subvec, "Reverse");
        tester.test_algorithm("MSDRadixSort", msd_radix_sort, reverse_subvec, "Reverse");
        tester.test_algorithm("MSDRadixSort+QS", msd_radix_sort_with_qs, reverse_subvec, "Reverse");
        
        tester.test_algorithm("StringQuickSort", string_quick_sort, nearly_subvec, "NearlySorted");
        tester.test_algorithm("StringMergeSort", string_merge_sort, nearly_subvec, "NearlySorted");
        tester.test_algorithm("MSDRadixSort", msd_radix_sort, nearly_subvec, "NearlySorted");
        tester.test_algorithm("MSDRadixSort+QS", msd_radix_sort_with_qs, nearly_subvec, "NearlySorted");
        
        tester.test_algorithm("StringQuickSort", string_quick_sort, prefix_subvec, "Prefix");
        tester.test_algorithm("StringMergeSort", string_merge_sort, prefix_subvec, "Prefix");
        tester.test_algorithm("MSDRadixSort", msd_radix_sort, prefix_subvec, "Prefix");
        tester.test_algorithm("MSDRadixSort+QS", msd_radix_sort_with_qs, prefix_subvec, "Prefix");
    }
    
    // Вывод и сохранение результатов
    tester.print_results();
    tester.export_results_csv("sorting_results.csv");
    
    cout << "\nResults saved to sorting_results.csv\n";
    cout << "You can use sorting_results.csv for plotting graphs.\n";
    
    return 0;
}