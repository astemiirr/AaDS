#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <random>
#include <cmath>
#include <algorithm>
#include <functional>
#include <cstdint>
#include <sys/stat.h>

// ==================== 1. RandomStreamGen ====================
class RandomStreamGen {
private:
    std::mt19937_64 gen;
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-";
    
public:
    RandomStreamGen(uint64_t seed = 42) : gen(seed) {}
    
    std::string generate_string() {
        std::uniform_int_distribution<int> len_dist(5, 30);
        std::uniform_int_distribution<int> char_dist(0, chars.size() - 1);
        
        int len = len_dist(gen);
        std::string result;
        for (int i = 0; i < len; ++i) {
            result += chars[char_dist(gen)];
        }
        return result;
    }
    
    std::vector<std::string> generate_stream(size_t size) {
        std::vector<std::string> stream;
        stream.reserve(size);
        
        for (size_t i = 0; i < size; ++i) {
            stream.push_back(generate_string());
        }
        return stream;
    }
    
    std::vector<std::vector<std::string>> split_stream(
        const std::vector<std::string>& stream, 
        const std::vector<double>& percentages) {
        
        std::vector<std::vector<std::string>> parts;
        size_t total = stream.size();
        
        size_t last_idx = 0;
        for (double p : percentages) {
            size_t end_idx = static_cast<size_t>(p * total);
            if (end_idx > total) end_idx = total;
            
            std::vector<std::string> part;
            for (size_t i = last_idx; i < end_idx; ++i) {
                part.push_back(stream[i]);
            }
            parts.push_back(part);
            last_idx = end_idx;
        }
        
        return parts;
    }
};

// ==================== 2. HashFuncGen ====================
class HashFuncGen {
private:
    uint32_t seed;
    
public:
    HashFuncGen(uint32_t seed = 42) : seed(seed) {}
    
    // FNV-1a хеш (хорошая равномерность)
    uint32_t fnv_hash(const std::string& s) const {
        const uint32_t FNV_OFFSET_BASIS = 2166136261u;
        const uint32_t FNV_PRIME = 16777619u;
        
        uint32_t hash = FNV_OFFSET_BASIS;
        for (char c : s) {
            hash ^= static_cast<uint8_t>(c);
            hash *= FNV_PRIME;
        }
        return hash;
    }
    
    // Семейство хеш-функций для HyperLogLog
    std::vector<std::pair<uint32_t, uint32_t>> generate_hash_family(size_t k) const {
        std::vector<std::pair<uint32_t, uint32_t>> family;
        std::mt19937 rng(seed);
        std::uniform_int_distribution<uint32_t> dist_a(1, 0x7FFFFFFF);
        std::uniform_int_distribution<uint32_t> dist_b(0, 0xFFFFFFFF);
        
        for (size_t i = 0; i < k; ++i) {
            family.emplace_back(dist_a(rng), dist_b(rng));
        }
        return family;
    }
    
    // Линейная комбинация для семейства
    uint32_t combined_hash(const std::string& s, uint32_t a, uint32_t b) const {
        uint32_t h = fnv_hash(s);
        // a * h + b mod 2^32
        return static_cast<uint32_t>(static_cast<uint64_t>(a) * h + b);
    }
    
    // Тест равномерности распределения
    void test_uniformity(const std::vector<std::string>& samples, size_t buckets = 100) const {
        std::vector<size_t> histogram(buckets, 0);
        
        for (const auto& s : samples) {
            uint32_t hash = fnv_hash(s);
            size_t bucket = static_cast<size_t>((static_cast<double>(hash) / UINT32_MAX) * buckets);
            if (bucket >= buckets) bucket = buckets - 1;
            histogram[bucket]++;
        }
        
        double expected = static_cast<double>(samples.size()) / buckets;
        double chi2 = 0.0;
        
        std::cout << "Тест равномерности FNV-1a хеша:" << std::endl;
        std::cout << "  Образцов: " << samples.size() << ", корзин: " << buckets << std::endl;
        
        for (size_t i = 0; i < buckets; ++i) {
            double diff = histogram[i] - expected;
            chi2 += (diff * diff) / expected;
        }
        
        std::cout << "  Chi-квадрат: " << chi2 << std::endl;
        std::cout << "  Ожидаемое (равномерное): " << buckets << " ± " << std::sqrt(2.0 * buckets) << std::endl;
        
        // Правило большого пальца: chi2 в пределах [buckets - 3√buckets, buckets + 3√buckets]
        double lower = buckets - 3 * std::sqrt(buckets);
        double upper = buckets + 3 * std::sqrt(buckets);
        
        if (chi2 >= lower && chi2 <= upper) {
            std::cout << "  Распределение равномерное" << std::endl;
        } else {
            std::cout << "  ✗ Распределение неравномерное" << std::endl;
        }
    }
};

// ==================== 3. HyperLogLog ====================
class HyperLogLog {
private:
    int b;  // бит для индекса регистра
    int m;  // количество регистров = 2^b
    std::vector<uint8_t> registers;
    std::function<uint32_t(const std::string&)> hash_func;
    
    // Подсчет ведущих нулей + 1
    int rho(uint32_t x) const {
        if (x == 0) return 32;
        int count = 0;
        while ((x & 0x80000000) == 0) {
            ++count;
            x <<= 1;
        }
        return count + 1;
    }
    
    // Поправочный коэффициент alpha
    double alpha() const {
        if (m == 16) return 0.673;
        if (m == 32) return 0.697;
        if (m == 64) return 0.709;
        return 0.7213 / (1.0 + 1.079 / m);
    }
    
public:
    HyperLogLog(int b, const std::function<uint32_t(const std::string&)>& hash_func)
        : b(b), m(1 << b), registers(m, 0), hash_func(hash_func) {}
    
    void add(const std::string& value) {
        uint32_t h = hash_func(value);
        int j = h >> (32 - b);        // первые b бит как индекс
        uint32_t w = h << b;          // оставшиеся биты
        uint8_t r = rho(w);
        
        if (r > registers[j]) {
            registers[j] = r;
        }
    }
    
    double estimate() const {
        double sum = 0.0;
        for (int i = 0; i < m; ++i) {
            sum += 1.0 / (1ULL << registers[i]);
        }
        
        double estimate_val = alpha() * m * m / sum;
        
        // Коррекции для малых и больших значений
        if (estimate_val <= 2.5 * m) {
            int zeros = 0;
            for (int i = 0; i < m; ++i) {
                if (registers[i] == 0) ++zeros;
            }
            if (zeros > 0) {
                estimate_val = m * std::log(static_cast<double>(m) / zeros);
            }
        }
        else if (estimate_val > (1ULL << 32) / 30.0) {
            estimate_val = - (1ULL << 32) * std::log(1.0 - estimate_val / (1ULL << 32));
        }
        
        return estimate_val;
    }
    
    // Слияние двух HyperLogLog (для проверки свойства)
    void merge(const HyperLogLog& other) {
        if (b != other.b) throw std::invalid_argument("Different b parameters");
        for (int i = 0; i < m; ++i) {
            if (other.registers[i] > registers[i]) {
                registers[i] = other.registers[i];
            }
        }
    }
    
    int get_b() const { return b; }
    int get_m() const { return m; }
};

// ==================== Вспомогательные функции ====================
bool create_directory(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return mkdir(path.c_str(), 0755) == 0;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

size_t exact_unique_count(const std::vector<std::string>& stream) {
    std::unordered_set<std::string> unique(stream.begin(), stream.end());
    return unique.size();
}

// ==================== Основная программа ====================
int main() {
    std::cout << "=== HyperLogLog Experiment ===" << std::endl;
    
    // Создаем директории для результатов
    if (!create_directory("data")) {
        std::cerr << "Ошибка создания директории data" << std::endl;
        return 1;
    }
    create_directory("results");
    
    try {
        // ========== ПАРАМЕТРЫ ЭКСПЕРИМЕНТА ==========
        const size_t STREAM_SIZE = 1000000;      // 1М элементов
        const std::vector<double> PERCENTAGES = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
        const int NUM_STREAMS = 5;               // 5 различных потоков
        const int B = 10;                        // 2^10 = 1024 регистра (1KB памяти)
        
        std::cout << "\nПараметры:" << std::endl;
        std::cout << "  Размер потока: " << STREAM_SIZE << " элементов" << std::endl;
        std::cout << "  Временные точки: ";
        for (double p : PERCENTAGES) std::cout << int(p*100) << "% ";
        std::cout << std::endl;
        std::cout << "  Количество потоков: " << NUM_STREAMS << std::endl;
        std::cout << "  Параметр B: " << B << " (m = " << (1 << B) << " регистров)" << std::endl;
        std::cout << "  Теоретическая ошибка: " << (1.04 / std::sqrt(1 << B) * 100) << "%" << std::endl;
        
        // ========== ЭТАП 1: ТЕСТ ХЕШ-ФУНКЦИИ ==========
        std::cout << "\n=== Этап 1: Тестирование хеш-функции ===" << std::endl;
        {
            RandomStreamGen test_rng(42);
            HashFuncGen hash_gen(42);
            
            auto test_samples = test_rng.generate_stream(10000);
            hash_gen.test_uniformity(test_samples);
        }
        
        // ========== ЭТАП 2: ОСНОВНОЙ ЭКСПЕРИМЕНТ ==========
        std::cout << "\n=== Этап 2: Основной эксперимент ===" << std::endl;
        
        std::ofstream results_csv("data/results.csv");
        results_csv << "stream_id,time_point,percentage,exact,estimate,error_percent\n";
        
        std::vector<std::vector<double>> estimates_by_time(PERCENTAGES.size());
        std::vector<std::vector<double>> exact_by_time(PERCENTAGES.size());
        
        for (int stream_id = 0; stream_id < NUM_STREAMS; ++stream_id) {
            std::cout << "\nПоток " << stream_id + 1 << "/" << NUM_STREAMS << ":" << std::endl;
            
            // Генерация потока
            RandomStreamGen rng(42 + stream_id);
            auto stream = rng.generate_stream(STREAM_SIZE);
            
            // Хеш-функция для этого потока
            HashFuncGen hash_gen(42 + stream_id);
            auto hash_family = hash_gen.generate_hash_family(1);
            auto hash_func = [&hash_gen, &hash_family](const std::string& s) -> uint32_t {
                return hash_gen.combined_hash(s, hash_family[0].first, hash_family[0].second);
            };
            
            // Создаем HyperLogLog
            HyperLogLog hll(B, hash_func);
            
            // Разбиваем поток на части
            auto parts = rng.split_stream(stream, PERCENTAGES);
            
            // Обрабатываем каждую часть
            for (size_t i = 0; i < parts.size(); ++i) {
                // Добавляем элементы текущей части
                for (const auto& s : parts[i]) {
                    hll.add(s);
                }
                
                // Собираем все элементы до текущего момента
                std::vector<std::string> current_stream;
                for (size_t j = 0; j <= i; ++j) {
                    current_stream.insert(current_stream.end(), 
                                         parts[j].begin(), parts[j].end());
                }
                
                // Точное значение
                size_t exact = exact_unique_count(current_stream);
                
                // Оценка HyperLogLog
                double estimate = hll.estimate();
                double error_percent = std::abs(estimate - exact) / exact * 100;
                
                // Сохраняем для статистик
                estimates_by_time[i].push_back(estimate);
                exact_by_time[i].push_back(static_cast<double>(exact));
                
                // Записываем в CSV
                results_csv << stream_id << ","
                           << int(PERCENTAGES[i] * 100) << ","
                           << PERCENTAGES[i] << ","
                           << exact << ","
                           << estimate << ","
                           << error_percent << "\n";
                
                std::cout << "  " << int(PERCENTAGES[i] * 100) << "%: "
                         << "exact=" << exact << ", "
                         << "est=" << static_cast<int>(estimate) << ", "
                         << "err=" << error_percent << "%" << std::endl;
            }
        }
        
        results_csv.close();
        std::cout << "\n Данные сохранены в data/results.csv" << std::endl;
        
        // ========== ЭТАП 3: СТАТИСТИЧЕСКИЙ АНАЛИЗ ==========
        std::cout << "\n=== Этап 3: Статистический анализ ===" << std::endl;
        
        std::ofstream stats_csv("data/statistics.csv");
        stats_csv << "percentage,exact_mean,exact_std,estimate_mean,estimate_std,error_mean,theoretical_error\n";
        
        for (size_t i = 0; i < PERCENTAGES.size(); ++i) {
            double exact_mean = 0.0, exact_std = 0.0;
            double estimate_mean = 0.0, estimate_std = 0.0;
            double error_sum = 0.0;
            
            // Вычисляем средние
            for (double val : exact_by_time[i]) exact_mean += val;
            for (double val : estimates_by_time[i]) estimate_mean += val;
            
            exact_mean /= exact_by_time[i].size();
            estimate_mean /= estimates_by_time[i].size();
            
            // Вычисляем стандартные отклонения
            for (double val : exact_by_time[i]) {
                double diff = val - exact_mean;
                exact_std += diff * diff;
            }
            for (double val : estimates_by_time[i]) {
                double diff = val - estimate_mean;
                estimate_std += diff * diff;
            }
            
            exact_std = std::sqrt(exact_std / exact_by_time[i].size());
            estimate_std = std::sqrt(estimate_std / estimates_by_time[i].size());
            
            // Средняя ошибка
            for (size_t j = 0; j < exact_by_time[i].size(); ++j) {
                error_sum += std::abs(estimates_by_time[i][j] - exact_by_time[i][j]) 
                           / exact_by_time[i][j];
            }
            double error_mean = error_sum / exact_by_time[i].size();
            
            // Теоретическая ошибка
            double theoretical_error = 1.04 / std::sqrt(1 << B);
            
            stats_csv << PERCENTAGES[i] << ","
                     << exact_mean << ","
                     << exact_std << ","
                     << estimate_mean << ","
                     << estimate_std << ","
                     << error_mean << ","
                     << theoretical_error << "\n";
            
            std::cout << "  " << int(PERCENTAGES[i] * 100) << "%, "
                     << "σ_exact=" << int(exact_std) << ", "
                     << "σ_est=" << int(estimate_std) << ", "
                     << "err=" << (error_mean * 100) << "%" << std::endl;
        }
        
        stats_csv.close();
        std::cout << " Статистики сохранены в data/statistics.csv" << std::endl;
        
        // ========== ЭТАП 4: АНАЛИЗ ВЫБОРА ПАРАМЕТРА B ==========
        std::cout << "\n=== Этап 4: Анализ параметра B ===" << std::endl;
        
        std::ofstream b_csv("data/b_analysis.csv");
        b_csv << "b,m,theoretical_error_104,theoretical_error_130,actual_error\n";
        
        // Тестовый поток для анализа B
        RandomStreamGen b_rng(999);
        auto test_stream = b_rng.generate_stream(50000);
        HashFuncGen b_hash_gen(999);
        auto b_hash_family = b_hash_gen.generate_hash_family(1);
        auto b_hash_func = [&b_hash_gen, &b_hash_family](const std::string& s) -> uint32_t {
            return b_hash_gen.combined_hash(s, b_hash_family[0].first, b_hash_family[0].second);
        };
        
        size_t exact_test = exact_unique_count(test_stream);
        
        for (int test_b = 4; test_b <= 16; ++test_b) {
            HyperLogLog test_hll(test_b, b_hash_func);
            
            for (const auto& s : test_stream) {
                test_hll.add(s);
            }
            
            double estimate = test_hll.estimate();
            double actual_error = std::abs(estimate - exact_test) / exact_test;
            double m = 1 << test_b;
            double theoretical_104 = 1.04 / std::sqrt(m);
            double theoretical_130 = 1.30 / std::sqrt(m);
            
            b_csv << test_b << ","
                  << m << ","
                  << theoretical_104 << ","
                  << theoretical_130 << ","
                  << actual_error << "\n";
            
            std::cout << "  B=" << test_b << " (m=" << m << "): "
                     << "err=" << (actual_error * 100) << "%, "
                     << "theor=" << (theoretical_104 * 100) << "%" << std::endl;
        }
        
        b_csv.close();
        std::cout << " Анализ B сохранен в data/b_analysis.csv" << std::endl;
        
        // ========== ЭТАП 5: ПРОВЕРКА СВОЙСТВА MERGE ==========
        std::cout << "\n=== Этап 5: Проверка свойства слияния ===" << std::endl;
        
        RandomStreamGen merge_rng(777);
        auto stream1 = merge_rng.generate_stream(50000);
        auto stream2 = merge_rng.generate_stream(50000);
        
        HashFuncGen merge_hash_gen(777);
        auto merge_hash_family = merge_hash_gen.generate_hash_family(1);
        auto merge_hash_func = [&merge_hash_gen, &merge_hash_family](const std::string& s) -> uint32_t {
            return merge_hash_gen.combined_hash(s, merge_hash_family[0].first, merge_hash_family[0].second);
        };
        
        HyperLogLog hll1(B, merge_hash_func);
        HyperLogLog hll2(B, merge_hash_func);
        HyperLogLog hll_merged(B, merge_hash_func);
        
        for (const auto& s : stream1) hll1.add(s);
        for (const auto& s : stream2) hll2.add(s);
        
        // Объединенный поток
        std::vector<std::string> merged_stream = stream1;
        merged_stream.insert(merged_stream.end(), stream2.begin(), stream2.end());
        for (const auto& s : merged_stream) hll_merged.add(s);
        
        // Слияние через merge()
        HyperLogLog hll_merge_test = hll1;
        hll_merge_test.merge(hll2);
        
        std::cout << "  HLL1: " << static_cast<int>(hll1.estimate()) << std::endl;
        std::cout << "  HLL2: " << static_cast<int>(hll2.estimate()) << std::endl;
        std::cout << "  Merge через функцию: " << static_cast<int>(hll_merge_test.estimate()) << std::endl;
        std::cout << "  Объединенный поток: " << static_cast<int>(hll_merged.estimate()) << std::endl;
        std::cout << "  Точное значение: " << exact_unique_count(merged_stream) << std::endl;
        
        // ========== ИТОГИ ==========
        std::cout << "\n=== ИТОГИ ===" << std::endl;
        std::cout << "1. Реализованы RandomStreamGen и HashFuncGen" << std::endl;
        std::cout << "2. Реализован HyperLogLog с оценкой Nt и точным подсчетом F0t" << std::endl;
        std::cout << "3. Проведен эксперимент с " << NUM_STREAMS << " потоками" << std::endl;
        std::cout << "4. Созданы файлы данных для визуализации" << std::endl;
        std::cout << "5. Выбран параметр B=" << B << " (обосновано анализом)" << std::endl;
        std::cout << "\nДля визуализации выполните: python3 visualization.py" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}