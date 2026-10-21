#pragma once

#include <cstddef>

// Вызывающая сторона владеет массивом. nullptr допустим только при size == 0.
// После array_remove_all выделенный блок может сохраниться при size == 0.
int* array_create(std::size_t size);
void array_delete(int*& arr);
// Старая память освобождается только после успешного выделения новой.
// Новые элементы при увеличении размера равны нулю.
int* array_resize(int* arr, std::size_t size, std::size_t new_size);
// При неверной позиции исходные массив и размер остаются прежними.
int* array_insert(int* arr, std::size_t& size, std::size_t pos, int value);
int* array_remove(int* arr, std::size_t& size, std::size_t pos);
void array_print(const int* arr, std::size_t size);

void array_gnome_sort(int* arr, std::size_t size);
// Массив должен быть отсортирован по возрастанию. Индекс первого вхождения.
// При неудаче выходные параметры не меняются.
bool array_binary_search(const int* arr, std::size_t size, int target,
                         std::size_t& out_index);
bool array_binary_search(const int* arr, std::size_t size, int target,
                         std::size_t& out_index, int& out_value);
// Удаление на месте без перевыделения; порядок остальных чисел сохраняется.
std::size_t array_remove_all(int* arr, std::size_t& size, int value);
