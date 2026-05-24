# A1 — Анализ строковых сортировок

Практическое исследование стандартных и адаптированных алгоритмов сортировки строк.

## Сборка и запуск

```bash
g++ -std=c++17 -O2 main.cpp string_generator.cpp string_sort_tester.cpp sort_metrics.cpp -o sort_test
./sort_test
```

Результаты сохраняются в `sorting_results.csv`.

## Графики (опционально)

```bash
pip install matplotlib pandas
python3 scripts/plot_results.py
```

PNG-файлы появятся в каталоге `figures/`.

## Структура проекта

| Файл | Назначение |
|------|------------|
| `string_generator.h/.cpp` | Генерация тестовых массивов |
| `string_sort_tester.h/.cpp` | Замеры времени и сравнений |
| `sort_metrics.h/.cpp` | Подсчёт посимвольных сравнений |
| `sort_common.h` | Общие константы (алфавит 74, pivot) |
| `standard_sorts.h` | QuickSort, MergeSort |
| `adapted_sorts.h` | String QS/MS, MSD, MSD+QS |
| `main.cpp` | Экспериментальный прогон |
| `codeforces/` | Отдельные решения для CF (A1m, A1q, A1r, A1rq) |
| `REPORT.md` | Отчёт с анализом |
| `SUBMISSION.md` | Чеклист сдачи |

## Параметры эксперимента

- Seed генератора: `42`
- Размеры: 100…3000, шаг 100
- Длина строк: 10…200
- Усреднение: 5 прогонов
- Типы массивов: Random, Reverse, NearlySorted, Prefix (общий префикс 50)
