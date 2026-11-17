#include "collection.h"

#include <stdexcept>
#include <utility>

Collection::Collection(const Collection& other) {
    Collection copy;
    copy.reserve(other.size_);
    for (std::size_t i = 0; i < other.size_; ++i) copy.add(*other.data_[i]);
    swap(copy);
}

Collection::Collection(Collection&& other) noexcept {
    swap(other);
}

Collection& Collection::operator=(const Collection& other) {
    if (this == &other) return *this;
    Collection copy(other);
    swap(copy);
    return *this;
}

Collection& Collection::operator=(Collection&& other) noexcept {
    if (this == &other) return *this;
    Collection moved(std::move(other));
    swap(moved);
    return *this;
}

Collection::~Collection() {
    for (std::size_t i = 0; i < size_; ++i) delete data_[i];
    delete[] data_;
}

void Collection::swap(Collection& other) noexcept {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

void Collection::reserve(std::size_t capacity) {
    if (capacity <= capacity_) return;
    Component** replacement = new Component*[capacity];
    for (std::size_t i = 0; i < size_; ++i) replacement[i] = data_[i];
    delete[] data_;
    data_ = replacement;
    capacity_ = capacity;
}

void Collection::add(const Component& item) {
    if (size_ == capacity_) reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    data_[size_] = item.clone();
    ++size_;
}

void Collection::remove(std::size_t index) {
    if (index >= size_) throw std::out_of_range("Invalid index");
    delete data_[index];
    for (std::size_t i = index + 1; i < size_; ++i) data_[i - 1] = data_[i];
    --size_;
    data_[size_] = nullptr;
}

double Collection::total() const {
    double result = 0;
    for (std::size_t i = 0; i < size_; ++i) result += data_[i]->metric();
    return result;
}

const Component& Collection::operator[](std::size_t index) const {
    if (index >= size_) throw std::out_of_range("Invalid index");
    return *data_[index];
}
