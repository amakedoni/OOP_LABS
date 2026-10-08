#include "array_ops.h"
#include <cctype>
#include <iostream>
#include <limits>
#include <new>

bool read_int(const char* prompt, int& value) {
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            int next = std::cin.peek();
            if (next == std::char_traits<char>::eof() ||
                std::isspace(static_cast<unsigned char>(next))) return true;
        }
        if (std::cin.eof() || std::cin.bad()) return false;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка ввода. Введите целое число.\n";
    }
}

int main() {
    int* arr = nullptr;
    std::size_t size = 0;
    bool running = true;
    std::cout << "Лабораторная 1. Вариант 4. Индексы с 0.\n";
    while (running) {
        std::cout << "\n1. Создать массив\n2. Напечатать\n3. Вставить элемент\n"
                     "4. Удалить элемент\n5. Изменить размер\n6. Печать\n"
                     "7. Гномья сортировка и удаление всех вхождений\n"
                     "8. Бинарный поиск\n0. Выход\n";
        int choice;
        if (!read_int("Ваш выбор: ", choice)) break;
        try {
            switch (choice) {
                case 0:
                    running = false;
                    break;
                case 1: {
                    int n;
                    if (!read_int("Размер: ", n)) { running = false; break; }
                    if (n < 0) { std::cout << "Размер не может быть отрицательным.\n"; break; }
                    int* temp = array_create(n);
                    for (int i = 0; i < n; ++i) {
                        if (!read_int("Элемент: ", temp[i])) { running = false; break; }
                    }
                    if (running) {
                        array_delete(arr);
                        arr = temp;
                        size = n;
                        array_print(arr, size);
                    } else {
                        array_delete(temp);
                    }
                    break;
                }
                case 2:
                case 6:
                    array_print(arr, size);
                    break;
                case 3: {
                    int pos, value;
                    if (!read_int("Позиция: ", pos)) { running = false; break; }
                    if (pos < 0 || static_cast<std::size_t>(pos) > size) {
                        std::cout << "Недопустимая позиция.\n";
                        break;
                    }
                    if (!read_int("Значение: ", value)) { running = false; break; }
                    arr = array_insert(arr, size, pos, value);
                    array_print(arr, size);
                    break;
                }
                case 4: {
                    if (size == 0) { std::cout << "Массив пуст.\n"; break; }
                    int pos;
                    if (!read_int("Индекс: ", pos)) { running = false; break; }
                    if (pos < 0 || static_cast<std::size_t>(pos) >= size) {
                        std::cout << "Недопустимая позиция.\n";
                        break;
                    }
                    arr = array_remove(arr, size, pos);
                    array_print(arr, size);
                    break;
                }
                case 5: {
                    int n;
                    if (!read_int("Новый размер: ", n)) { running = false; break; }
                    if (n < 0) { std::cout << "Размер не может быть отрицательным.\n"; break; }
                    arr = array_resize(arr, size, n);
                    size = n;
                    array_print(arr, size);
                    break;
                }
                case 7: {
                    int value;
                    if (!read_int("Удалить все вхождения числа: ", value)) { running = false; break; }
                    array_gnome_sort(arr, size);
                    std::cout << "После сортировки:\n";
                    array_print(arr, size);
                    std::size_t removed = array_remove_all(arr, size, value);
                    std::cout << "Удалено элементов: " << removed << '\n';
                    array_print(arr, size);
                    break;
                }
                case 8: {
                    int value;
                    if (!read_int("Искомое число: ", value)) { running = false; break; }
                    array_gnome_sort(arr, size);
                    array_print(arr, size);
                    std::size_t index;
                    if (array_binary_search(arr, size, value, index)) {
                        std::cout << "Найдено: индекс " << index << ", значение " << arr[index] << '\n';
                    } else {
                        std::cout << "Не найдено.\n";
                    }
                    break;
                }
                default:
                    std::cout << "Неизвестный пункт меню.\n";
            }
        } catch (const std::bad_alloc&) {
            std::cout << "Не хватает памяти. Массив сохранён.\n";
        }
    }
    array_delete(arr);
    return 0;
}
