#include "matrix.h"

#include <limits>
#include <ostream>
#include <stdexcept>
#include <utility>

Matrix::Matrix(std::size_t rows, std::size_t cols, double fill)
    : rows_(rows), cols_(cols) {
    const auto max_elements = std::numeric_limits<std::size_t>::max() / sizeof(double);
    if (cols != 0 && rows > max_elements / cols) {
        throw std::length_error("Matrix size is too large");
    }
    const std::size_t count = rows * cols;
    if (count != 0) {
        data_ = new double[count];
        for (std::size_t i = 0; i < count; ++i) {
            data_[i] = fill;
        }
    }
}

Matrix::~Matrix() { delete[] data_; }

Matrix::Matrix(const Matrix& other) : Matrix(other.rows_, other.cols_) {
    for (std::size_t i = 0; i < rows_ * cols_; ++i) {
        data_[i] = other.data_[i];
    }
}

Matrix::Matrix(Matrix&& other) {
    swap(other);
}

Matrix& Matrix::operator=(Matrix other) {
    swap(other);
    return *this;
}

void Matrix::swap(Matrix& other) {
    std::swap(rows_, other.rows_);
    std::swap(cols_, other.cols_);
    std::swap(data_, other.data_);
}

double Matrix::get(std::size_t r, std::size_t c) const {
    if (r >= rows_ || c >= cols_) {
        throw std::out_of_range("Matrix index is out of range");
    }
    return data_[r * cols_ + c];
}

void Matrix::set(std::size_t r, std::size_t c, double value) {
    if (r >= rows_ || c >= cols_) {
        throw std::out_of_range("Matrix index is out of range");
    }
    data_[r * cols_ + c] = value;
}

void Matrix::add(const Matrix& other) {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        throw std::invalid_argument("Addition requires equal matrix dimensions");
    }
    for (std::size_t i = 0; i < rows_ * cols_; ++i) {
        data_[i] += other.data_[i];
    }
}

void Matrix::mul(const Matrix& other) {
    if (cols_ != other.rows_) {
        throw std::invalid_argument("Multiplication requires left.cols == right.rows");
    }
    Matrix result(rows_, other.cols_);
    if (cols_ != 0 && other.cols_ != 0) {
        for (std::size_t r = 0; r < rows_; ++r) {
            for (std::size_t c = 0; c < other.cols_; ++c) {
                double sum = 0.0;
                for (std::size_t k = 0; k < cols_; ++k) {
                    sum += data_[r * cols_ + k] * other.data_[k * other.cols_ + c];
                }
                result.data_[r * other.cols_ + c] = sum;
            }
        }
    }
    swap(result);
}

bool Matrix::is_symmetric() const {
    if (rows_ != cols_) return false;
    for (std::size_t r = 0; r < rows_; ++r) {
        for (std::size_t c = r; c < cols_; ++c) {
            if (data_[r * cols_ + c] != data_[c * cols_ + r]) return false;
        }
    }
    return true;
}

void Matrix::print(std::ostream& out) const {
    out << '[';
    for (std::size_t r = 0; r < rows_; ++r) {
        if (r != 0) out << ", ";
        out << '[';
        for (std::size_t c = 0; c < cols_; ++c) {
            if (c != 0) out << ", ";
            out << data_[r * cols_ + c];
        }
        out << ']';
    }
    out << ']';
}
