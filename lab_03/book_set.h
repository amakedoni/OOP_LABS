#pragma once

#include "book.h"

#include <cstddef>

class BookSet {
public:
    BookSet() = default;
    BookSet(const BookSet& other);
    BookSet(BookSet&& other) noexcept;
    BookSet& operator=(const BookSet& other);
    BookSet& operator=(BookSet&& other) noexcept;
    ~BookSet();

    bool add(const Book& book);
    bool remove(const Book& book);
    bool contains(const Book& book) const;
    bool operator()(const Book& book) const { return contains(book); }
    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    BookSet& operator+=(const Book& book);
    BookSet& operator-=(const Book& book);
    BookSet operator|(const BookSet& other) const;
    BookSet operator&(const BookSet& other) const;
    BookSet operator-(const BookSet& other) const;
    bool operator==(const BookSet& other) const;
    bool operator!=(const BookSet& other) const { return !(*this == other); }

    // Только чтение: изменение книги нарушило бы порядок элементов.
    const Book& operator[](std::size_t index) const;
    friend std::ostream& operator<<(std::ostream& out, const BookSet& set);

private:
    void swap(BookSet& other) noexcept;
    void reserve(std::size_t capacity);
    void append(const Book& book);
    std::size_t find_position(const Book& book) const;
    BookSet select(const BookSet& other, bool keep_common) const;

    Book* data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
};
