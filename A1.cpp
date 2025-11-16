#include <iostream>
#include <random>
#include <cmath>
#include <vector>
#include <iomanip>
#include <fstream>

using namespace std;

// Точное значение площади
const double EXACT_AREA = 0.25 * M_PI + 1.25 * asin(0.8) - 1.0;

// Проверка попадания точки в пересечение трёх кругов
bool is_in_intersection(double x, double y) {
    double r1_dist = (x - 1) * (x - 1) + (y - 1) * (y - 1);
    double r2_dist = (x - 1.5) * (x - 1.5) + (y - 2) * (y - 2);
    double r3_dist = (x - 2) * (x - 2) + (y - 1.5) * (y - 1.5);

    return (r1_dist <= 1.0) &&
        (r2_dist <= 1.25) &&
        (r3_dist <= 1.25);
}

// Метод Монте-Карло
double Monte_Carlo(double x_min, double x_max, double y_min, double y_max, int N) {
    // Рандомайзер
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dist_x(x_min, x_max);
    uniform_real_distribution<double> dist_y(y_min, y_max);

    int M = 0;
    for (int i = 0; i < N; ++i) {
        double x = dist_x(gen);
        double y = dist_y(gen);
        if (is_in_intersection(x, y)) {
            ++M;
        }
    }

    double rect_area = (x_max - x_min) * (y_max - y_min);
    return (static_cast<double>(M) / N) * rect_area;
}

// Функция для построения графиков с помощью GNUplot
void plot_results(const vector<int>& N_values, const vector<double>& areas_big,
    const vector<double>& areas_tiny, const vector<double>& deviations_wide,
    const vector<double>& deviations_narrow) {

    // Записываем данные в файл
    ofstream data_file("monte_carlo_data.txt");
    data_file << fixed << setprecision(6);
    data_file << "N Area_Wide Area_Narrow Error_Wide Error_Narrow\n";

    for (size_t i = 0; i < N_values.size(); ++i) {
        data_file << N_values[i] << " " << areas_big[i] << " " << areas_tiny[i]
            << " " << deviations_wide[i] << " " << deviations_narrow[i] << "\n";
    }
    data_file.close();

    // Скрипт для GNUplot - График 1: Приближенные значения площади
    ofstream plot_script1("plot_areas.gnu");
    plot_script1 << "set terminal png size 1200,800 enhanced font 'Arial,12'\n";
    plot_script1 << "set output 'monte_carlo_areas.png'\n";
    plot_script1 << "set title 'Метод Монте-Карло: Приближенная площадь пересечения кругов'\n";
    plot_script1 << "set xlabel 'Количество точек N'\n";
    plot_script1 << "set ylabel 'Площадь'\n";
    plot_script1 << "set grid\n";
    plot_script1 << "set key top right\n";
    plot_script1 << "plot " << EXACT_AREA << " title 'Точное значение' with lines lw 3 lc 'black', \\\n";
    plot_script1 << "     'monte_carlo_data.txt' using 1:2 title 'Широкая область' with lines lw 2 lc 'blue', \\\n";
    plot_script1 << "     'monte_carlo_data.txt' using 1:3 title 'Узкая область' with lines lw 2 lc 'red'\n";
    plot_script1.close();

    // Скрипт для GNUplot - График 2: Относительные ошибки
    ofstream plot_script2("plot_errors.gnu");
    plot_script2 << "set terminal png size 1200,800 enhanced font 'Arial,12'\n";
    plot_script2 << "set output 'monte_carlo_errors.png'\n";
    plot_script2 << "set title 'Метод Монте-Карло: Относительная ошибка'\n";
    plot_script2 << "set xlabel 'Количество точек N'\n";
    plot_script2 << "set ylabel 'Абсолютная ошибка'\n";
    plot_script2 << "set grid\n";
    plot_script2 << "set key top right\n";
    plot_script2 << "set logscale y\n";  // Логарифмическая шкала для лучшего отображения
    plot_script2 << "plot 'monte_carlo_data.txt' using 1:4 title 'Широкая область' with lines lw 2 lc 'blue', \\\n";
    plot_script2 << "     'monte_carlo_data.txt' using 1:5 title 'Узкая область' with lines lw 2 lc 'red'\n";
    plot_script2.close();

    // Запускаем GNUplot
    system("gnuplot plot_areas.gnu");
    system("gnuplot plot_errors.gnu");

    cout << "Графики сохранены как 'monte_carlo_areas.png' и 'monte_carlo_errors.png'\n";
}

int main() {
    // Широкая область
    double big_xmin = 0.0, big_xmax = 3.15;
    double big_ymin = 0.0, big_ymax = 3.15;

    // Узкая область
    double tiny_xmin = 0.85, tiny_xmax = 2.05;
    double tiny_ymin = 0.85, tiny_ymax = 2.05;

    // Векторы для хранения результатов
    vector<int> N_values;
    vector<double> areas_big, areas_tiny;
    vector<double> deviations_wide, deviations_narrow;

    for (int N = 100; N <= 100000; N += 500) {
        double area_big = Monte_Carlo(big_xmin, big_xmax, big_ymin, big_ymax, N);
        double area_tiny = Monte_Carlo(tiny_xmin, tiny_xmax, tiny_ymin, tiny_ymax, N);

        double deviation_big = fabs(area_big - EXACT_AREA);
        double deviation_tiny = fabs(area_tiny - EXACT_AREA);

        // Сохраняем результаты
        N_values.push_back(N);
        areas_big.push_back(area_big);
        areas_tiny.push_back(area_tiny);
        deviations_wide.push_back(deviation_big);
        deviations_narrow.push_back(deviation_tiny);

    }

    // Строим графики
    plot_results(N_values, areas_big, areas_tiny, deviations_wide, deviations_narrow);

    return 0;
}