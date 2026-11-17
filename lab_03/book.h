#pragma once

#include <iostream>

class Book {
public:
    Book() = default;
    Book(const char* title, int year);
    Book(const Book& other);
    Book(Book&& other) noexcept;
    Book& operator=(const Book& other);
    Book& operator=(Book&& other) noexcept;
    ~Book();

    const char* title() const { return title_ == nullptr ? "" : title_; }

    bool operator==(const Book& other) const;
    bool operator<(const Book& other) const;
    friend std::ostream& operator<<(std::ostream& out, const Book& book);

private:
    void swap(Book& other) noexcept;
    char* title_ = nullptr;
    int year_ = 0;
};
