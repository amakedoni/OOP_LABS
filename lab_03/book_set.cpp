#include "book_set.h"

#include <utility>

BookSet::BookSet(const BookSet& other) {
    BookSet copy;
    copy.reserve(other.size_);
    for (std::size_t i = 0; i < other.size_; ++i) copy.append(other.data_[i]);
    swap(copy);
}

BookSet::BookSet(BookSet&& other) noexcept {
    swap(other);
}

BookSet& BookSet::operator=(const BookSet& other) {
    if (this == &other) return *this;
    BookSet copy(other);
    swap(copy);
    return *this;
}

BookSet& BookSet::operator=(BookSet&& other) noexcept {
    if (this == &other) return *this;
    BookSet moved(std::move(other));
    swap(moved);
    return *this;
}

BookSet::~BookSet() { delete[] data_; }

void BookSet::swap(BookSet& other) noexcept {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

void BookSet::reserve(std::size_t capacity) {
    if (capacity <= capacity_) return;
    Book* replacement = new Book[capacity];
    for (std::size_t i = 0; i < size_; ++i) {
        replacement[i] = std::move(data_[i]);
    }
    delete[] data_;
    data_ = replacement;
    capacity_ = capacity;
}

// Место для элемента заранее выделено через reserve.
void BookSet::append(const Book& book) {
    data_[size_] = book;
    ++size_;
}

std::size_t BookSet::find_position(const Book& book) const {
    std::size_t left = 0;
    std::size_t right = size_;
    while (left < right) {
        const std::size_t middle = left + (right - left) / 2;
        if (data_[middle] < book) {
            left = middle + 1;
        } else {
            right = middle;
        }
    }
    return left;
}

bool BookSet::add(const Book& book) {
    const std::size_t position = find_position(book);
    if (position < size_ && data_[position] == book) return false;

    Book copy(book);
    if (size_ == capacity_) {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    for (std::size_t i = size_; i > position; --i) {
        data_[i] = std::move(data_[i - 1]);
    }
    data_[position] = std::move(copy);
    ++size_;
    return true;
}

bool BookSet::remove(const Book& book) {
    const std::size_t position = find_position(book);
    if (position == size_ || !(data_[position] == book)) return false;
    for (std::size_t i = position; i + 1 < size_; ++i) {
        data_[i] = std::move(data_[i + 1]);
    }
    --size_;
    data_[size_] = Book();
    return true;
}

bool BookSet::contains(const Book& book) const {
    const std::size_t position = find_position(book);
    return position < size_ && data_[position] == book;
}

BookSet& BookSet::operator+=(const Book& book) {
    add(book);
    return *this;
}

BookSet& BookSet::operator-=(const Book& book) {
    remove(book);
    return *this;
}

BookSet BookSet::operator|(const BookSet& other) const {
    BookSet result;
    result.reserve(size_ + other.size_);
    std::size_t j = 0;
    for (std::size_t i = 0; i < size_; ++i) {
        const Book& book = data_[i];
        while (j < other.size_ && other.data_[j] < book) {
            result.append(other.data_[j++]);
        }
        result.append(book);
        if (j < other.size_ && other.data_[j] == book) ++j;
    }
    while (j < other.size_) result.append(other.data_[j++]);
    return result;
}

BookSet BookSet::operator&(const BookSet& other) const {
    return select(other, true);
}

BookSet BookSet::operator-(const BookSet& other) const {
    return select(other, false);
}

BookSet BookSet::select(const BookSet& other, bool keep_common) const {
    BookSet result;
    const std::size_t max_size = keep_common && other.size_ < size_ ? other.size_ : size_;
    result.reserve(max_size);
    std::size_t j = 0;
    for (std::size_t i = 0; i < size_; ++i) {
        const Book& book = data_[i];
        while (j < other.size_ && other.data_[j] < book) ++j;
        const bool common = j < other.size_ && other.data_[j] == book;
        if (common == keep_common) result.append(book);
    }
    return result;
}

bool BookSet::operator==(const BookSet& other) const {
    if (size_ != other.size_) return false;
    for (std::size_t i = 0; i < size_; ++i) {
        if (!(data_[i] == other.data_[i])) return false;
    }
    return true;
}

const Book& BookSet::operator[](std::size_t index) const {
    if (index >= size_) throw "BookSet: index out of range";
    return data_[index];
}

std::ostream& operator<<(std::ostream& out, const BookSet& set) {
    if (set.empty()) return out << "{}";
    out << "{ " << set.data_[0];
    for (std::size_t i = 1; i < set.size_; ++i) out << ", " << set.data_[i];
    return out << " }";
}
