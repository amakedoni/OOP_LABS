if(SCENARIO STREQUAL "variant")
    set(input "1\n6\n2 5 2 8 2 1\n7\n2\n8\n5\n8\n9\n0\n")
    set(expected
        "Массив: {1, 2, 2, 2, 5, 8}"
        "Удалено элементов: 3"
        "Массив: {1, 5, 8}"
        "Найдено: индекс 1, значение 5"
        "Элемент не найден.")
elseif(SCENARIO STREQUAL "basic")
    set(input "1\n3\n5 1 -2\n3\n1\n9\n4\n0\n5\n5\n5\n2\n2\n6\n0\n")
    set(expected "Массив: {5, 9, 1, -2}" "Массив: {9, 1, -2}"
        "Массив: {9, 1, -2, 0, 0}" "Массив: {9, 1}")
elseif(SCENARIO STREQUAL "invalid")
    set(input "abc\n99999999999999999999999999999\n12\n1\n-2\n2x\n2\n1.5\n7\n8\n3\n5\n4\n-1\n2\n2\n0\n")
    set(expected "Ошибка ввода. Введите целое число."
        "Неизвестный пункт меню." "Введите неотрицательное целое число."
        "Недопустимая позиция." "Массив: {7, 8}")
elseif(SCENARIO STREQUAL "empty")
    set(input "2\n4\n7\n2\n8\n42\n1\n0\n5\n0\n0\n")
    set(expected "Массив: {}" "Массив пуст." "Удалено элементов: 0"
        "Элемент не найден.")
elseif(SCENARIO STREQUAL "eof_create")
    set(input "1\n2\n11 22\n1\n3\n4")
    set(expected "Массив: {11, 22}")
elseif(SCENARIO STREQUAL "eof_menu")
    set(input "")
    set(expected "Лабораторная работа 1. Вариант 4.")
elseif(SCENARIO STREQUAL "eof_invalid")
    set(input "x")
    set(expected "Ошибка ввода. Введите целое число.")
elseif(SCENARIO STREQUAL "no_newline")
    set(input "0")
    set(expected "Лабораторная работа 1. Вариант 4.")
else()
    message(FATAL_ERROR "Unknown menu scenario: ${SCENARIO}")
endif()

set(input_file "${CMAKE_CURRENT_BINARY_DIR}/menu-${SCENARIO}.txt")
file(WRITE "${input_file}" "${input}")
execute_process(COMMAND "${PROGRAM}"
    INPUT_FILE "${input_file}" OUTPUT_VARIABLE output ERROR_VARIABLE errors
    RESULT_VARIABLE result TIMEOUT 10)
file(REMOVE "${input_file}")
if(NOT "${result}" STREQUAL "0")
    message(FATAL_ERROR "Program failed: ${result}\n${output}\n${errors}")
endif()
if(NOT "${errors}" STREQUAL "")
    message(FATAL_ERROR "Unexpected stderr: ${errors}")
endif()
list(APPEND expected "Завершение работы.")
foreach(fragment IN LISTS expected)
    string(FIND "${output}" "${fragment}" position)
    if(position EQUAL -1)
        message(FATAL_ERROR "Missing: ${fragment}\nActual output:\n${output}")
    endif()
endforeach()
