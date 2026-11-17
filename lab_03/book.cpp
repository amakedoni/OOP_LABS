#include "book.h"

#include <cstring>
#include <utility>

Book::Book(const char* title, int year) : year_(year) {
    if (title == nullptr) title = "";
    title_ = new char[std::strlen(title) + 1];
    std::strcpy(title_, title);
}

Book::Book(const Book& other) : Book(other.title(), other.year_) {}

Book::Book(Book&& other) noexcept {
    swap(other);
}

Book& Book::operator=(const Book& other) {
    if (this == &other) return *this;
    Book copy(other);
    swap(copy);
    return *this;
}

Book& Book::operator=(Book&& other) noexcept {
    if (this == &other) return *this;
    Book moved(std::move(other));
    swap(moved);
    return *this;
}

Book::~Book() { delete[] title_; }

void Book::swap(Book& other) noexcept {
    std::swap(title_, other.title_);
    std::swap(year_, other.year_);
}

bool Book::operator==(const Book& other) const {
    return std::strcmp(title(), other.title()) == 0 && year_ == other.year_;
}

bool Book::operator<(const Book& other) const {
    const int order = std::strcmp(title(), other.title());
    return order < 0 || (order == 0 && year_ < other.year_);
}

std::ostream& operator<<(std::ostream& out, const Book& book) {
    return out << "(\"" << book.title() << "\", " << book.year_ << ')';
}
