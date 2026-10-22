#ifndef LAB02_MATRIX_H
#define LAB02_MATRIX_H

#include <cstddef>
#include <iosfwd>

class Matrix {
public:
    Matrix() = default;
    Matrix(std::size_t rows, std::size_t cols, double fill = 0.0);
    Matrix(const Matrix& other);
    Matrix(Matrix&& other);
    Matrix& operator=(Matrix other);
    ~Matrix();

    void swap(Matrix& other);
    std::size_t rows() const { return rows_; }
    std::size_t cols() const { return cols_; }
    double get(std::size_t r, std::size_t c) const;
    void set(std::size_t r, std::size_t c, double value);
    void add(const Matrix& other);
    void mul(const Matrix& other);
    bool is_symmetric() const;
    void print(std::ostream& out) const;

private:
    std::size_t rows_ = 0;
    std::size_t cols_ = 0;
    double* data_ = nullptr;
};

#endif
