#include "matrix.h"

#include <cerrno>
#include <charconv>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <new>
#include <stdexcept>

namespace {
// Читаем целый токен, в том числе слишком длинный, без динамических контейнеров.
bool read_token(char (&text)[128]) {
    std::cin >> std::ws;
    if (!std::cin.good()) return false;
    std::size_t length = 0;
    bool too_long = false;
    char ch;
    while (std::cin.get(ch)) {
        if (std::isspace(static_cast<unsigned char>(ch))) break;
        if (length + 1 < sizeof(text)) text[length++] = ch;
        else too_long = true;
    }
    text[length] = '\0';
    if (too_long) text[0] = '\0';
    return true;
}

bool read_size(const char* prompt, std::size_t& value) {
    char text[128];
    while (true) {
        std::cout << prompt;
        if (!read_token(text)) return false;
        std::size_t length = 0;
        while (text[length] != '\0') ++length;
        const auto result = std::from_chars(text, text + length, value);
        if (result.ec == std::errc{} && result.ptr == text + length) return true;
        std::cout << "Ошибка ввода: нужно целое неотрицательное число.\n";
    }
}

bool read_value(const char* prompt, double& value) {
    char text[128];
    while (true) {
        std::cout << prompt;
        if (!read_token(text)) return false;
        char* end = nullptr;
        errno = 0;
        value = std::strtod(text, &end);
        if (end != text && *end == '\0' && errno != ERANGE && std::isfinite(value)) {
            return true;
        }
        std::cout << "Ошибка ввода: нужно конечное число, дробная часть через точку.\n";
    }
}

bool read_matrix(Matrix& target) {
    std::size_t rows, cols;
    if (!read_size("Количество строк: ", rows) ||
        !read_size("Количество столбцов: ", cols)) return false;
    Matrix result(rows, cols);
    std::cout << "Введите элементы по строкам (можно через пробел):\n";
    if (cols != 0) {
        for (std::size_t r = 0; r < rows; ++r) {
            for (std::size_t c = 0; c < cols; ++c) {
                double value;
                if (!read_value("Элемент: ", value)) return false;
                result.set(r, c, value);
            }
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

void show_symmetry(const Matrix& matrix) {
    std::cout << "Симметрична: " << (matrix.is_symmetric() ? "да" : "нет") << '\n';
}

bool edit(Matrix& matrix) {
    std::size_t row, col;
    double value;
    if (!read_size("Строка (с 0): ", row) ||
        !read_size("Столбец (с 0): ", col) ||
        !read_value("Новое значение: ", value)) return false;
    matrix.set(row, col, value);
    show(matrix);
    return true;
}

void demonstrate() {
    const Matrix a(2, 2, 1.0);
    const Matrix b(2, 2, 2.0);
    Matrix sum(a);
    sum.add(b);
    std::cout << "Сумма: ";
    sum.print(std::cout);
    Matrix product(a);
    product.mul(b);
    std::cout << "\nПроизведение: ";
    product.print(std::cout);
    std::cout << '\n';

    Matrix symmetric(2, 2, 1.0);
    symmetric.set(0, 1, 2.0);
    symmetric.set(1, 0, 2.0);
    show(symmetric);
    show_symmetry(symmetric);
    Matrix asymmetric(symmetric);
    asymmetric.set(1, 0, 3.0);
    asymmetric.set(1, 1, 4.0);
    show(asymmetric);
    show_symmetry(asymmetric);
}

void menu() {
    std::cout << "\n1. Создать матрицу\n2. Напечатать матрицу\n"
                 "3. Изменить элемент\n4. Прибавить матрицу\n"
                 "5. Умножить на матрицу\n6. Проверить симметричность\n"
                 "7. Примеры из задания\n0. Выход\n";
}
}

int main() {
    std::cout << "Лабораторная работа 2. Семейство C, вариант 16. Matrix\n";
    Matrix matrix;
    bool running = true;
    while (running) {
        menu();
        std::size_t choice;
        if (!read_size("Пункт: ", choice)) break;
        try {
            switch (choice) {
            case 0: running = false; break;
            case 1:
                running = read_matrix(matrix);
                if (running) show(matrix);
                break;
            case 2: show(matrix); break;
            case 3: running = edit(matrix); break;
            case 4:
            case 5: {
                std::cout << "Введите вторую матрицу:\n";
                Matrix other;
                running = read_matrix(other);
                if (!running) break;
                if (choice == 4) matrix.add(other);
                else matrix.mul(other);
                show(matrix);
                break;
            }
            case 6: show_symmetry(matrix); break;
            case 7: demonstrate(); break;
            default: std::cout << "Неизвестный пункт меню.\n"; break;
            }
        } catch (const std::bad_alloc&) {
            std::cout << "Ошибка: недостаточно памяти.\n";
        } catch (const std::exception& error) {
            std::cout << "Ошибка: " << error.what() << '\n';
        }
    }
    std::cout << "\nРабота завершена.\n";
}
