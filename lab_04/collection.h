#pragma once

#include "component.h"
#include <cstddef>

class Collection {
public:
    Collection() = default;
    Collection(const Collection& other);
    Collection(Collection&& other) noexcept;
    Collection& operator=(const Collection& other);
    Collection& operator=(Collection&& other) noexcept;
    ~Collection();

    void add(const Component& item);
    void remove(std::size_t index);
    std::size_t size() const { return size_; }
    double total() const;
    const Component& operator[](std::size_t index) const;

private:
    void swap(Collection& other) noexcept;
    void reserve(std::size_t capacity);
    Component** data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
};
