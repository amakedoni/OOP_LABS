#include "matrix.h"

#include <ostream>
#include <utility>

Matrix::Matrix(std::size_t rows, std::size_t cols, double fill) {
    rows_ = rows;
    cols_ = cols;
    const std::size_t count = rows * cols;
    if (count != 0) {
        data_ = new double[count];
        for (std::size_t i = 0; i < count; ++i) {
            data_[i] = fill;
        }
    }
}

Matrix::~Matrix() { delete[] data_; }

Matrix::Matrix(const Matrix& other) {
    rows_ = other.rows_;
    cols_ = other.cols_;
    const std::size_t count = rows_ * cols_;
    if (count != 0) {
        data_ = new double[count];
        for (std::size_t i = 0; i < count; ++i) {
            data_[i] = other.data_[i];
        }
    }
}

Matrix::Matrix(Matrix&& other) {
    rows_ = other.rows_;
    cols_ = other.cols_;
    data_ = other.data_;
    other.rows_ = 0;
    other.cols_ = 0;
    other.data_ = nullptr;
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
    return data_[r * cols_ + c];
}

void Matrix::set(std::size_t r, std::size_t c, double value) {
    data_[r * cols_ + c] = value;
}

void Matrix::add(const Matrix& other) {
		
    for (std::size_t i = 0; i < rows_ * cols_; ++i) {
        data_[i] += other.data_[i];
    }
}

void Matrix::mul(const Matrix& other) {
    Matrix result(rows_, other.cols_);
    for (std::size_t r = 0; r < rows_; ++r) {
        for (std::size_t c = 0; c < other.cols_; ++c) {
            double sum = 0.0;
            for (std::size_t k = 0; k < cols_; ++k) {
                sum += get(r, k) * other.get(k, c);
            }
            result.set(r, c, sum);
        }
    }
    swap(result);
}

bool Matrix::is_symmetric() const {
    if (rows_ != cols_) return false;
    for (std::size_t r = 0; r < rows_; ++r) {
        for (std::size_t c = r; c < cols_; ++c) {
            if (get(r, c) != get(c, r)) return false;
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
            out << get(r, c);
        }
        out << ']';
    }
    out << ']';
}
