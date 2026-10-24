#include "matrix.h"

#include <cctype>
#include <cmath>
#include <iostream>
#include <limits>

bool number_finished() {
    const int next = std::cin.peek();
    return next == std::char_traits<char>::eof() || std::isspace(static_cast<unsigned char>(next));
}

bool read_size(const char* prompt, std::size_t& value) {
    while (true) {
        std::cout << prompt;
        int number = 0;
        if (std::cin >> number) {
            if (number >= 0 && number_finished()) {
                value = static_cast<std::size_t>(number);
                return true;
            }
        }
        if (std::cin.eof() || std::cin.bad()) return false;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка ввода: нужно целое неотрицательное число.\n";
    }
}

bool read_value(const char* prompt, double& value) {
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            if (std::isfinite(value) && number_finished()) return true;
        }
        if (std::cin.eof() || std::cin.bad()) return false;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка ввода: нужно конечное число, дробная часть через точку.\n";
    }
}

bool read_matrix(Matrix& target) {
    std::size_t rows, cols;
    if (!read_size("Количество строк: ", rows)) return false;
    if (!read_size("Количество столбцов: ", cols)) return false;
    Matrix result(rows, cols);
    std::cout << "Введите элементы по строкам (можно через пробел):\n";
    for (std::size_t r = 0; r < rows; ++r) {
        for (std::size_t c = 0; c < cols; ++c) {
            double value;
            if (!read_value("", value)) return false;
            result.set(r, c, value);
        }
    }
    target.swap(result);
    return true;
}

void show(const Matrix& matrix) {
    std::cout << "Матрица: ";
    matrix.print(std::cout);
    std::cout << "\nРазмер: " << matrix.rows() << 'x' << matrix.cols() << '\n';
}

void menu() {
    std::cout << "\n1. Создать матрицу\n"
                 "2. Напечатать матрицу\n"
                 "3. Изменить элемент\n"
                 "4. Прибавить матрицу\n"
                 "5. Умножить на матрицу\n"
                 "6. Проверить симметричность\n"
                 "0. Выход\n";
}

int main() {
    std::cout << "Лабораторная работа 2. Семейство C, вариант 16. Matrix\n";
    Matrix matrix;
    while (true) {
        menu();
        std::size_t choice;
        if (!read_size("Пункт: ", choice)) break;
        if (choice == 0) break;
        if (choice == 1) {
            if (!read_matrix(matrix)) break;
            show(matrix);
        } else if (choice == 2) {
            show(matrix);
        } else if (choice == 3) {
            std::size_t row, col;
            double value;
            if (!read_size("Строка (с 0): ", row)) break;
            if (!read_size("Столбец (с 0): ", col)) break;
            if (!read_value("Новое значение: ", value)) break;
            if (row >= matrix.rows() || col >= matrix.cols()) {
                std::cout << "Ошибка: нет элемента с таким индексом.\n";
                continue;
            }
            matrix.set(row, col, value);
            show(matrix);
        } else if (choice == 4 || choice == 5) {
            std::cout << "Введите вторую матрицу:\n";
            Matrix other;
            if (!read_matrix(other)) break;
            if (choice == 4) {
                if (matrix.rows() != other.rows() || matrix.cols() != other.cols()) {
                    std::cout << "Ошибка: для сложения размеры должны совпадать.\n";
                    continue;
                }
                matrix.add(other);
            } else {
                if (matrix.cols() != other.rows()) {
                    std::cout << "Ошибка: для умножения нужны совместимые размеры.\n";
                    continue;
                }
                matrix.mul(other);
            }
            show(matrix);
        } else if (choice == 6) {
            if (matrix.is_symmetric()) {
                std::cout << "Симметрична: да\n";
            } else {
                std::cout << "Симметрична: нет\n";
            }
        } else {
            std::cout << "Неизвестный пункт меню.\n";
        }
    }
    std::cout << "\nРабота завершена.\n";
}
