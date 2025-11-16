#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <string>
#include <cmath>
#include <map>

using namespace std;

class ArrayGenerator {
private:
    mt19937 gen;
    uniform_int_distribution<int> dist;

public:
    ArrayGenerator() : gen(random_device{}()), dist(0, 6000) {}

    vector<int> generate_random_vec(size_t size) {
        vector<int> vec(size);
        for (size_t i = 0; i < size; ++i) {
            vec[i] = dist(gen);
        }
        return vec;
    }

    vector<int> generate_reverse_sorted_vec(int size) {
        vector<int> vec = generate_random_vec(size);
        sort(vec.begin(), vec.end(), greater<int>());
        return vec;
    }

    vector<int> generate_almost_sorted_vec(int size, int swap_count = 10) {
        vector<int> vec(size);
        for (int i = 0; i < size; ++i) {
            vec[i] = i;
        }

        uniform_int_distribution<int> index_dist(0, size - 1);
        for (int i = 0; i < swap_count; ++i) {
            int idx1 = index_dist(gen);
            int idx2 = index_dist(gen);
            swap(vec[idx1], vec[idx2]);
        }
        return vec;
    }
};

void merge(vector<int>& vec, int left, int mid, int right) {
    vector<int> temp(right - left + 1);
    int i = left, j = mid + 1, k = 0;

    while (i <= mid && j <= right) {
        if (vec[i] <= vec[j]) {
            temp[k++] = vec[i++];
        }
        else {
            temp[k++] = vec[j++];
        }
    }

    while (i <= mid) temp[k++] = vec[i++];
    while (j <= right) temp[k++] = vec[j++];

    for (int idx = 0; idx < k; ++idx) {
        vec[left + idx] = temp[idx];
    }
}

void insertionSort(vector<int>& vec, int left, int right) {
    for (int i = left + 1; i <= right; ++i) {
        int key = vec[i];
        int j = i - 1;
        while (j >= left && vec[j] > key) {
            vec[j + 1] = vec[j];
            --j;
        }
        vec[j + 1] = key;
    }
}

void mergeSort(vector<int>& vec, int left, int right) {
    if (left >= right) return;

    int mid = left + (right - left) / 2;
    mergeSort(vec, left, mid);
    mergeSort(vec, mid + 1, right);
    merge(vec, left, mid, right);
}

void hybridMergeSort(vector<int>& vec, int left, int right, int threshold) {
    if (right - left + 1 <= threshold) {
        insertionSort(vec, left, right);
        return;
    }

    int mid = left + (right - left) / 2;
    hybridMergeSort(vec, left, mid, threshold);
    hybridMergeSort(vec, mid + 1, right, threshold);
    merge(vec, left, mid, right);
}

class SortTester {
private:
    ArrayGenerator generator;

public:
    vector<long long> testMergeSort(vector<int>& input_vec, int iterations = 3) {
        vector<long long> times;

        for (int i = 0; i < iterations; ++i) {
            vector<int> vec = input_vec; // Копируем массив для теста

            auto start = chrono::high_resolution_clock::now();
            mergeSort(vec, 0, vec.size() - 1);
            auto finish = chrono::high_resolution_clock::now() - start;

            long long duration = chrono::duration_cast<chrono::milliseconds>(finish).count();
            times.push_back(duration);
        }

        return times;
    }

    vector<long long> testHybridMergeSort(vector<int>& input_vec, int threshold, int iterations = 3) {
        vector<long long> times;

        for (int i = 0; i < iterations; ++i) {
            vector<int> arr = input_vec; // Копируем массив для теста

            auto start = chrono::high_resolution_clock::now();
            hybridMergeSort(arr, 0, arr.size() - 1, threshold);
            auto finish = chrono::high_resolution_clock::now() - start;

            long long duration = chrono::duration_cast<chrono::milliseconds>(finish).count();
            times.push_back(duration);
        }

        return times;
    }

    long long getMedianTime(const vector<long long>& times) {
        if (times.empty()) return 0;
        vector<long long> sorted_times = times;
        sort(sorted_times.begin(), sorted_times.end());
        return sorted_times[sorted_times.size() / 2];
    }

    void generateGnuplotScripts() {
        // График 1: Стандартная сортировка для разных типов массивов
        ofstream script1("plot_standard.gnu");
        script1 << "set terminal png size 1200,800 enhanced font 'Arial,12'\n";
        script1 << "set output 'standard_performance.png'\n";
        script1 << "set title 'Standard Merge Sort Performance'\n";
        script1 << "set xlabel 'Array Size'\n";
        script1 << "set ylabel 'Time (milliseconds)'\n";
        script1 << "set grid\n";
        script1 << "set key top left\n";
        script1 << "plot 'data_random.txt' using 1:2 with lines title 'Random' lw 2 lc 'blue', \\\n";
        script1 << "     'data_reverse.txt' using 1:2 with lines title 'Reverse Sorted' lw 2 lc 'red', \\\n";
        script1 << "     'data_almost.txt' using 1:2 with lines title 'Almost Sorted' lw 2 lc 'green'\n";
        script1.close();

        // График 2: Сравнение стандартной и гибридной сортировки
        ofstream script2("plot_comparison.gnu");
        script2 << "set terminal png size 1200,800 enhanced font 'Arial,12'\n";
        script2 << "set output 'comparison.png'\n";
        script2 << "set title 'Merge Sort vs Hybrid Merge Sort (Random Arrays)'\n";
        script2 << "set xlabel 'Array Size'\n";
        script2 << "set ylabel 'Time (milliseconds)'\n";
        script2 << "set grid\n";
        script2 << "set key top left\n";
        script2 << "plot 'data_random.txt' using 1:2 with lines title 'Standard' lw 3 lc 'black', \\\n";
        script2 << "     'data_random.txt' using 1:4 with lines title 'Hybrid (threshold=10)' lw 2 lc 'blue'\n";
        script2.close();

        // График 3: Анализ разных порогов
        ofstream script3("plot_thresholds.gnu");
        script3 << "set terminal png size 1200,800 enhanced font 'Arial,12'\n";
        script3 << "set output 'threshold_analysis.png'\n";
        script3 << "set title 'Hybrid Sort Performance for Different Thresholds (Random Arrays)'\n";
        script3 << "set xlabel 'Array Size'\n";
        script3 << "set ylabel 'Time (milliseconds)'\n";
        script3 << "set grid\n";
        script3 << "set key top left\n";
        script3 << "plot 'data_random.txt' using 1:2 with lines title 'Standard' lw 3 lc 'black', \\\n";
        script3 << "     'data_random.txt' using 1:3 with lines title 'Threshold=5' lw 2 lc 'red', \\\n";
        script3 << "     'data_random.txt' using 1:4 with lines title 'Threshold=10' lw 2 lc 'blue', \\\n";
        script3 << "     'data_random.txt' using 1:5 with lines title 'Threshold=20' lw 2 lc 'green', \\\n";
        script3 << "     'data_random.txt' using 1:6 with lines title 'Threshold=30' lw 2 lc 'orange', \\\n";
        script3 << "     'data_random.txt' using 1:7 with lines title 'Threshold=50' lw 2 lc 'purple'\n";
        script3.close();

        // График 4: Оптимальный порог
        ofstream script4("plot_optimal.gnu");
        script4 << "set terminal png size 1200,800 enhanced font 'Arial,12'\n";
        script4 << "set output 'optimal_threshold.png'\n";
        script4 << "set title 'Optimal Threshold Analysis'\n";
        script4 << "set xlabel 'Array Size'\n";
        script4 << "set ylabel 'Optimal Threshold'\n";
        script4 << "set yrange [0:60]\n";
        script4 << "set grid\n";
        script4 << "plot 'optimal_threshold.txt' using 1:2 with linespoints title 'Optimal Threshold' lw 2\n";
        script4.close();
    }

    void runTests() {
        cout << "Generating test data..." << endl;

        // Создаем отдельные файлы для каждого типа данных
        ofstream dataRandom("data_random.txt");
        ofstream dataReverse("data_reverse.txt");
        ofstream dataAlmost("data_almost.txt");
        ofstream optimalFile("optimal_threshold.txt");

        if (!dataRandom.is_open() || !dataReverse.is_open() || !dataAlmost.is_open()) {
            cerr << "Error: Cannot open data files!" << endl;
            return;
        }

        // Генерируем ОДИН РАЗ массивы максимальной длины для каждого типа
        cout << "Generating base arrays of size 100000..." << endl;
        vector<int> random_base = generator.generate_random_vec(100000);
        vector<int> reverse_base = generator.generate_reverse_sorted_vec(100000);
        vector<int> almost_base = generator.generate_almost_sorted_vec(100000);

        vector<int> thresholds = { 5, 10, 20, 30, 50 };

        // Тестируем от 500 до 100000 с шагом 100
        for (int size = 500; size <= 100000; size += 100) {
            if (size % 5000 == 0) {
                cout << "Testing size: " << size << "/100000" << endl;
            }

            // Берем подмассивы из базовых массивов
            vector<int> random_array(random_base.begin(), random_base.begin() + size);
            vector<int> reverse_array(reverse_base.begin(), reverse_base.begin() + size);
            vector<int> almost_array(almost_base.begin(), almost_base.begin() + size);

            // Тестируем случайные массивы
            auto random_std = testMergeSort(random_array, 3);
            long long random_std_time = getMedianTime(random_std);

            dataRandom << size << " " << random_std_time;
            vector<long long> random_hybrid_times;
            for (int threshold : thresholds) {
                auto times = testHybridMergeSort(random_array, threshold, 3);
                long long hybrid_time = getMedianTime(times);
                dataRandom << " " << hybrid_time;
                random_hybrid_times.push_back(hybrid_time);
            }
            dataRandom << "\n";

            // Анализ оптимального порога для случайных массивов
            long long best_time = random_std_time;
            int best_threshold = 0;

            for (size_t i = 0; i < random_hybrid_times.size(); ++i) {
                if (random_hybrid_times[i] < best_time) {
                    best_time = random_hybrid_times[i];
                    best_threshold = thresholds[i];
                }
            }

            if (best_threshold > 0) {
                optimalFile << size << " " << best_threshold << "\n";
            }

            // Тестируем обратно отсортированные массивы
            auto reverse_std = testMergeSort(reverse_array, 3);
            long long reverse_std_time = getMedianTime(reverse_std);

            dataReverse << size << " " << reverse_std_time;
            for (int threshold : thresholds) {
                auto times = testHybridMergeSort(reverse_array, threshold, 3);
                long long hybrid_time = getMedianTime(times);
                dataReverse << " " << hybrid_time;
            }
            dataReverse << "\n";

            // Тестируем почти отсортированные массивы
            auto almost_std = testMergeSort(almost_array, 3);
            long long almost_std_time = getMedianTime(almost_std);

            dataAlmost << size << " " << almost_std_time;
            for (int threshold : thresholds) {
                auto times = testHybridMergeSort(almost_array, threshold, 3);
                long long hybrid_time = getMedianTime(times);
                dataAlmost << " " << hybrid_time;
            }
            dataAlmost << "\n";
        }

        dataRandom.close();
        dataReverse.close();
        dataAlmost.close();
        optimalFile.close();

        // Генерируем скрипты GNUplot
        generateGnuplotScripts();

        // Запускаем GNUplot
        cout << "Generating plots..." << endl;
        system("gnuplot plot_standard.gnu");
        system("gnuplot plot_comparison.gnu");
        system("gnuplot plot_thresholds.gnu");
        system("gnuplot plot_optimal.gnu");

        cout << "Test completed! Generated PNG files:" << endl;
        cout << "- standard_performance.png" << endl;
        cout << "- comparison.png" << endl;
        cout << "- threshold_analysis.png" << endl;
        cout << "- optimal_threshold.png" << endl;

        // Удаляем временные файлы
        system("rm -f data_random.txt data_reverse.txt data_almost.txt");
        system("rm -f plot_standard.gnu plot_comparison.gnu plot_thresholds.gnu plot_optimal.gnu");
    }
};

int main() {
    // Проверяем наличие GNUplot
    if (system("which gnuplot > /dev/null 2>&1") != 0) {
        cerr << "Error: GNUplot is not installed. Please install it with:" << endl;
        cerr << "sudo apt-get install gnuplot" << endl;
        return 1;
    }

    SortTester tester;
    cout << "Starting sorting algorithms performance test..." << endl;
    tester.runTests();
    return 0;
}