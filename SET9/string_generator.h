// string_generator.h
#ifndef STRING_GENERATOR_H
#define STRING_GENERATOR_H

#include <vector>
#include <string>
#include <algorithm>
#include <random>

class StringGenerator {
private:
    static const std::string ALPHABET;
    std::mt19937 rng;
    
public:
    enum ArrayType {
        RANDOM,
        REVERSE_SORTED,
        NEARLY_SORTED
    };
    
    StringGenerator(int seed = 42);
    
    // Генерировать одну строку по заданной длине
    std::string generateString(int minLen = 10, int maxLen = 200);
    
    // Генерировать массив строк
    std::vector<std::string> generateArray(int size, int minLen = 10, int maxLen = 200);
    
    // Генерировать массив с общим префиксом
    std::vector<std::string> generateWithCommonPrefix(int size, int prefixLen, int minLen = 10, int maxLen = 200);
    
    // Генерировать массив заданного типа
    std::vector<std::string> generateTypedArray(int size, ArrayType type, int minLen = 10, int maxLen = 200);
    
    // Создать подмассив
    std::vector<std::string> getSubarray(const std::vector<std::string>& arr, int size);
};

#endif