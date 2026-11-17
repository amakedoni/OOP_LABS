#include "book_set.h"

bool read_book(Book& book) {
    char title[256];
    int year;
    std::cout << "Название: ";
    std::cin >> std::ws;
    if (!std::cin.getline(title, sizeof(title))) return false;
    std::cout << "Год: ";
    if (!(std::cin >> year)) return false;
    book = Book(title, year);
    return true;
}

bool read_set(BookSet& set) {
    int count;
    std::cout << "Количество книг: ";
    if (!(std::cin >> count) || count < 0) return false;
    set = BookSet();
    for (int i = 0; i < count; ++i) {
        std::cout << "Книга " << i + 1 << ":\n";
        Book book;
        if (!read_book(book)) return false;
        set += book;
    }
    return true;
}

void show(const BookSet& set) {
    std::cout << "Множество: " << set << "\nРазмер: " << set.size() << '\n';
}

int main() {
    BookSet books;
    int choice;
    while (true) {
        std::cout << "\n1. Создать множество\n2. Напечатать множество\n"
                     "3. Добавить книгу\n4. Удалить книгу\n5. Принадлежность\n"
                     "6. Объединение\n7. Пересечение\n8. Разность\n"
                     "9. Сравнение\n10. Книга по индексу\n0. Выход\nПункт: ";
        if (!(std::cin >> choice) || choice == 0) break;
        if (choice == 1 || choice == 2) {
            if (choice == 1 && !read_set(books)) break;
            show(books);
        } else if (choice >= 3 && choice <= 5) {
            Book book;
            if (!read_book(book)) break;
            if (choice == 5) {
                std::cout << (books(book) ? "Принадлежит: да\n" : "Принадлежит: нет\n");
                continue;
            }
            if (choice == 3) books += book;
            else books -= book;
            show(books);
        } else if (choice >= 6 && choice <= 9) {
            std::cout << "Введите множество B:\n";
            BookSet other;
            if (!read_set(other)) break;
            if (choice == 6) std::cout << "A | B: " << (books | other) << '\n';
            if (choice == 7) std::cout << "A & B: " << (books & other) << '\n';
            if (choice == 8) std::cout << "A - B: " << (books - other) << '\n';
            if (choice == 9) std::cout << (books == other ? "Равны: да\n" : "Равны: нет\n");
        } else if (choice == 10) {
            std::size_t index;
            std::cout << "Индекс (с 0): ";
            if (!(std::cin >> index)) break;
            if (index < books.size()) {
                std::cout << "Книга[" << index << "]: " << books[index] << '\n';
            } else {
                std::cout << "Неверный индекс\n";
            }
        } else {
            std::cout << "Неизвестный пункт меню.\n";
        }
    }
}
