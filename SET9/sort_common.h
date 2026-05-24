#ifndef SORT_COMMON_H
#define SORT_COMMON_H

// Мощность алфавита из задания (74 символа)
constexpr int ALPHABET_SIZE = 74;

// Порог переключения MSD Radix -> String QuickSort (по условию)
constexpr int MSD_QS_CUTOFF = ALPHABET_SIZE;

// Индекс опорного элемента (одинаков для стандартного и строкового QuickSort)
inline int pivotIndex(int low, int high) {
    return low + (high - low) / 2;
}

#endif
