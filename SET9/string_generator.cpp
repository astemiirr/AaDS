// string_generator.cpp
#include "string_generator.h"

const std::string StringGenerator::ALPHABET = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "!@#%:;^&*()-";

StringGenerator::StringGenerator(int seed) : rng(seed) {}

std::string StringGenerator::generateString(int minLen, int maxLen) {
    std::uniform_int_distribution<int> lenDist(minLen, maxLen);
    std::uniform_int_distribution<int> charDist(0, ALPHABET.size() - 1);
    
    int length = lenDist(rng);
    std::string result;
    result.reserve(length);
    
    for (int i = 0; i < length; ++i) {
        result += ALPHABET[charDist(rng)];
    }
    
    return result;
}

std::vector<std::string> StringGenerator::generateArray(int size, 
                                                         int minLen, 
                                                         int maxLen) {
    std::vector<std::string> result;
    result.reserve(size);
    
    for (int i = 0; i < size; ++i) {
        result.push_back(generateString(minLen, maxLen));
    }
    
    return result;
}

std::vector<std::string> StringGenerator::generateWithCommonPrefix(
    int size, int prefixLen, int minLen, int maxLen) {
    
    std::vector<std::string> result;
    result.reserve(size);
    
    // Генерируем общий префикс
    std::string commonPrefix = generateString(prefixLen, prefixLen);
    
    for (int i = 0; i < size; ++i) {
        int suffixLen = std::max(minLen - prefixLen, 1);
        std::string suffix = generateString(suffixLen, maxLen - prefixLen);
        result.push_back(commonPrefix + suffix);
    }
    
    return result;
}

std::vector<std::string> StringGenerator::generateTypedArray(
    int size, ArrayType type, int minLen, int maxLen) {
    
    auto arr = generateArray(size, minLen, maxLen);
    
    switch (type) {
        case RANDOM:
            break;
            
        case REVERSE_SORTED:
            std::sort(arr.begin(), arr.end(), std::greater<std::string>());
            break;
            
        case NEARLY_SORTED:
            std::sort(arr.begin(), arr.end());
            // Делаем несколько перестановок (1/20 от размера)
            int swaps = std::max(1, size / 20);
            std::uniform_int_distribution<int> idxDist(0, size - 1);
            for (int i = 0; i < swaps; ++i) {
                int a = idxDist(rng);
                int b = idxDist(rng);
                std::swap(arr[a], arr[b]);
            }
            break;
    }
    
    return arr;
}

std::vector<std::string> StringGenerator::getSubarray(
    const std::vector<std::string>& arr, int size) {
    
    return std::vector<std::string>(arr.begin(), arr.begin() + size);
}