#ifndef LAB02_MATRIX_H
#define LAB02_MATRIX_H

#include <cstddef>
#include <iosfwd>

class Matrix {
public:
    Matrix() noexcept = default;
    Matrix(std::size_t rows, std::size_t cols, double fill = 0.0);
    Matrix(const Matrix& other);
    Matrix(Matrix&& other) noexcept;
    Matrix& operator=(Matrix other) noexcept;
    ~Matrix();

    void swap(Matrix& other) noexcept;
    [[nodiscard]] std::size_t rows() const noexcept { return rows_; }
    [[nodiscard]] std::size_t cols() const noexcept { return cols_; }
    [[nodiscard]] double get(std::size_t r, std::size_t c) const;
    void set(std::size_t r, std::size_t c, double value);
    void add(const Matrix& other);
    void mul(const Matrix& other);
    [[nodiscard]] bool is_symmetric() const noexcept;
    void print(std::ostream& out) const;

private:
    std::size_t rows_ = 0;
    std::size_t cols_ = 0;
    double* data_ = nullptr;
};

#endif
