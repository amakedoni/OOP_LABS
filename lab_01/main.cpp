#include "array_ops.h"

#include <cctype>
#include <iostream>
#include <limits>
#include <new>

namespace {
bool read_int(const char* prompt, int& value) {
    while (true) {
        std::cout << prompt;
        int parsed = 0;
        if (std::cin >> parsed) {
            const int next = std::cin.peek();
            if (next == std::char_traits<char>::eof() ||
                std::isspace(static_cast<unsigned char>(next))) {
                value = parsed;
                return true;
            }
        }
        if (std::cin.eof() || std::cin.bad()) return false;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка ввода. Введите целое число.\n";
    }
}

bool read_size(const char* prompt, std::size_t& size) {
    int value = 0;
    while (read_int(prompt, value)) {
        if (value >= 0) {
            size = static_cast<std::size_t>(value);
            return true;
        }
        std::cout << "Введите неотрицательное целое число.\n";
    }
    return false;
}

bool create_array(int*& arr, std::size_t& size) {
    std::size_t new_size = 0;
    if (!read_size("Размер: ", new_size)) return false;
    int* created = array_create(new_size);
    if (new_size != 0) {
        std::cout << "Введите " << new_size << " целых чисел (через пробел или по одному на строке):\n";
    }
    for (std::size_t i = 0; i < new_size; ++i) {
        if (!read_int("", created[i])) {
            array_delete(created);
            return false;
        }
    }
    array_delete(arr);
    arr = created;
    size = new_size;
    array_print(arr, size);
    return true;
}

void print_menu() {
    std::cout << "\n1. Создать массив\n"
                 "2. Напечатать массив\n"
                 "3. Вставить элемент\n"
                 "4. Удалить элемент по индексу\n"
                 "5. Изменить размер\n"
                 "6. Печать\n"
                 "7. Гномья сортировка и удаление всех вхождений\n"
                 "8. Бинарный поиск\n"
                 "0. Выход\n";
}
}

int main() {
    int* arr = nullptr;
    std::size_t size = 0;
    bool running = true;
    std::cout << "Лабораторная работа 1. Вариант 4.\n"
                 "Индексы начинаются с 0.\n";

    while (running) {
        print_menu();
        int choice = 0;
        if (!read_int("Ваш выбор: ", choice)) break;
        try {
            switch (choice) {
                case 0:
                    running = false;
                    break;
                case 1:
                    running = create_array(arr, size);
                    break;
                case 2:
                case 6:
                    array_print(arr, size);
                    break;
                case 3: {
                    std::size_t pos = 0;
                    int value = 0;
                    if (!read_size("Позиция вставки: ", pos)) {
                        running = false;
                        break;
                    }
                    if (pos > size) {
                        std::cout << "Недопустимая позиция.\n";
                        break;
                    }
                    if (!read_int("Значение: ", value)) {
                        running = false;
                        break;
                    }
                    arr = array_insert(arr, size, pos, value);
                    array_print(arr, size);
                    break;
                }
                case 4: {
                    if (size == 0) {
                        std::cout << "Массив пуст.\n";
                        break;
                    }
                    std::size_t pos = 0;
                    if (!read_size("Индекс удаления: ", pos)) {
                        running = false;
                        break;
                    }
                    if (pos >= size) {
                        std::cout << "Недопустимая позиция.\n";
                        break;
                    }
                    arr = array_remove(arr, size, pos);
                    array_print(arr, size);
                    break;
                }
                case 5: {
                    std::size_t new_size = 0;
                    if (!read_size("Новый размер: ", new_size)) {
                        running = false;
                        break;
                    }
                    arr = array_resize(arr, size, new_size);
                    size = new_size;
                    array_print(arr, size);
                    break;
                }
                case 7: {
                    int value = 0;
                    if (!read_int("Удалить все вхождения числа: ", value)) {
                        running = false;
                        break;
                    }
                    array_gnome_sort(arr, size);
                    std::cout << "После сортировки:\n";
                    array_print(arr, size);
                    const std::size_t removed = array_remove_all(arr, size, value);
                    std::cout << "Удалено элементов: " << removed << '\n';
                    array_print(arr, size);
                    break;
                }
                case 8: {
                    int target = 0;
                    if (!read_int("Искомое число: ", target)) {
                        running = false;
                        break;
                    }
                    array_gnome_sort(arr, size);
                    std::cout << "Массив для бинарного поиска:\n";
                    array_print(arr, size);
                    std::size_t index = 0;
                    int value = 0;
                    if (array_binary_search(arr, size, target, index, value)) {
                        std::cout << "Найдено: индекс " << index
                                  << ", значение " << value << '\n';
                    } else {
                        std::cout << "Элемент не найден.\n";
                    }
                    break;
                }
                default:
                    std::cout << "Неизвестный пункт меню.\n";
            }
        } catch (const std::bad_alloc&) {
            std::cout << "Не удалось выделить память. Исходный массив сохранён.\n";
        }
    }

    array_delete(arr);
    std::cout << "Завершение работы.\n";
    return 0;
}
