#include "matrix.h"

#include <gtest/gtest.h>
#include <utility>

TEST(MatrixStorage, DefaultMatrixIsEmpty) {
    const Matrix matrix;
    EXPECT_EQ(matrix.rows(), 0u);
    EXPECT_EQ(matrix.cols(), 0u);
}

TEST(MatrixStorage, ConstructorFillsEveryElement) {
    const Matrix matrix(2, 3, -2.5);
    ASSERT_EQ(matrix.rows(), 2u);
    ASSERT_EQ(matrix.cols(), 3u);
    for (std::size_t r = 0; r < 2; ++r) {
        for (std::size_t c = 0; c < 3; ++c) {
            EXPECT_DOUBLE_EQ(matrix.get(r, c), -2.5);
        }
    }
}

TEST(MatrixStorage, DefaultFillIsZero) {
    const Matrix matrix(1, 2);
    EXPECT_DOUBLE_EQ(matrix.get(0, 0), 0.0);
    EXPECT_DOUBLE_EQ(matrix.get(0, 1), 0.0);
}

TEST(MatrixStorage, SetChangesOnlySelectedElement) {
    Matrix matrix(2, 3, 1.0);
    matrix.set(1, 2, 9.5);
    EXPECT_DOUBLE_EQ(matrix.get(1, 2), 9.5);
    EXPECT_DOUBLE_EQ(matrix.get(0, 2), 1.0);
    EXPECT_DOUBLE_EQ(matrix.get(1, 1), 1.0);
}

TEST(MatrixStorage, ZeroDimensionsKeepShapeWithoutElements) {
    const Matrix no_rows(0, 3);
    const Matrix no_cols(2, 0);
    EXPECT_EQ(no_rows.rows(), 0u);
    EXPECT_EQ(no_rows.cols(), 3u);
    EXPECT_EQ(no_cols.rows(), 2u);
    EXPECT_EQ(no_cols.cols(), 0u);
}

TEST(MatrixOwnership, CopyConstructorCreatesIndependentBuffer) {
    Matrix source(2, 2, 3.0);
    source.set(1, 0, 8.0);
    Matrix copy(source);
    ASSERT_EQ(copy.rows(), 2u);
    ASSERT_EQ(copy.cols(), 2u);
    EXPECT_DOUBLE_EQ(copy.get(1, 0), 8.0);
    copy.set(1, 0, 99.0);
    EXPECT_DOUBLE_EQ(source.get(1, 0), 8.0);
}

TEST(MatrixOwnership, CopyAssignmentReplacesShapeAndDoesNotShareStorage) {
    Matrix source(1, 3, 4.0);
    Matrix target(2, 1, -1.0);
    EXPECT_EQ(&(target = source), &target);
    ASSERT_EQ(target.rows(), 1u);
    ASSERT_EQ(target.cols(), 3u);
    source.set(0, 1, 99.0);
    EXPECT_DOUBLE_EQ(target.get(0, 1), 4.0);
}

TEST(MatrixOwnership, CopySurvivesDestructionOfSource) {
    Matrix target;
    {
        const Matrix source(1, 1, 6.0);
        target = source;
    }
    ASSERT_EQ(target.rows(), 1u);
    EXPECT_DOUBLE_EQ(target.get(0, 0), 6.0);
}

TEST(MatrixOwnership, SelfCopyAssignmentPreservesElements) {
    Matrix matrix(2, 2, 5.0);
    Matrix& alias = matrix;
    matrix = alias;
    EXPECT_EQ(matrix.rows(), 2u);
    EXPECT_DOUBLE_EQ(matrix.get(1, 1), 5.0);
}

TEST(MatrixOwnership, MoveConstructorEmptiesAndAllowsReusingSource) {
    Matrix source(2, 3, 7.0);
    Matrix target(std::move(source));
    ASSERT_EQ(target.rows(), 2u);
    ASSERT_EQ(target.cols(), 3u);
    EXPECT_DOUBLE_EQ(target.get(1, 2), 7.0);
    EXPECT_EQ(source.rows(), 0u);
    EXPECT_EQ(source.cols(), 0u);
    source = Matrix(1, 1, 9.0);
    ASSERT_EQ(source.rows(), 1u);
    EXPECT_DOUBLE_EQ(source.get(0, 0), 9.0);
}

TEST(MatrixOwnership, MoveAssignmentReleasesPreviousBufferAndEmptiesSource) {
    Matrix source(1, 3, 8.0);
    Matrix target(3, 3, 2.0);
    target = std::move(source);
    ASSERT_EQ(target.rows(), 1u);
    ASSERT_EQ(target.cols(), 3u);
    EXPECT_DOUBLE_EQ(target.get(0, 2), 8.0);
    EXPECT_EQ(source.rows(), 0u);
    EXPECT_EQ(source.cols(), 0u);
}

TEST(MatrixOwnership, SelfMoveAssignmentRemainsUsable) {
    Matrix matrix(1, 2, 4.0);
    Matrix& alias = matrix;
    matrix = std::move(alias);
    EXPECT_EQ(matrix.rows(), 1u);
    EXPECT_DOUBLE_EQ(matrix.get(0, 1), 4.0);
}

TEST(MatrixOwnership, SwapExchangesShapeAndData) {
    Matrix first(1, 2, 3.0);
    Matrix second(3, 1, 4.0);
    first.swap(second);
    ASSERT_EQ(first.rows(), 3u);
    ASSERT_EQ(first.cols(), 1u);
    ASSERT_EQ(second.rows(), 1u);
    ASSERT_EQ(second.cols(), 2u);
    EXPECT_DOUBLE_EQ(first.get(2, 0), 4.0);
    EXPECT_DOUBLE_EQ(second.get(0, 1), 3.0);
    first.swap(first);
    EXPECT_DOUBLE_EQ(first.get(2, 0), 4.0);
}

TEST(MatrixOwnership, EmptyObjectsCanBeCopiedMovedAndAssigned) {
    const Matrix empty;
    Matrix copy(empty);
    Matrix moved(std::move(copy));
    Matrix filled(2, 2, 1.0);
    filled = empty;
    EXPECT_EQ(moved.rows(), 0u);
    EXPECT_EQ(copy.cols(), 0u);
    EXPECT_EQ(filled.rows(), 0u);
}

TEST(MatrixOwnership, CopyAndMovePreserveZeroDimensionShape) {
    const Matrix source(0, 3);
    Matrix copy(source);
    Matrix target(std::move(copy));
    EXPECT_EQ(target.rows(), 0u);
    EXPECT_EQ(target.cols(), 3u);
    EXPECT_EQ(copy.cols(), 0u);
}

Matrix example(double lower_left = 2.0) {
    Matrix matrix(2, 2);
    matrix.set(0, 0, 1.0);
    matrix.set(0, 1, 2.0);
    matrix.set(1, 0, lower_left);
    matrix.set(1, 1, 4.0);
    return matrix;
}

TEST(MatrixArithmetic, AdditionMatchesPdfAndPreservesRightOperand) {
    Matrix left(2, 2, 1.0);
    const Matrix right(2, 2, 2.0);
    left.add(right);
    EXPECT_DOUBLE_EQ(left.get(0, 0), 3.0);
    EXPECT_DOUBLE_EQ(left.get(1, 1), 3.0);
    EXPECT_DOUBLE_EQ(right.get(0, 1), 2.0);
}

TEST(MatrixArithmetic, RectangularAdditionUsesAllElements) {
    Matrix left(2, 3, -1.0);
    Matrix right(2, 3, 2.0);
    right.set(1, 2, 5.0);
    left.add(right);
    EXPECT_DOUBLE_EQ(left.get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(left.get(1, 2), 4.0);
}

TEST(MatrixArithmetic, SelfAdditionDoublesEveryElement) {
    Matrix matrix = example(3.0);
    matrix.add(matrix);
    EXPECT_DOUBLE_EQ(matrix.get(0, 0), 2.0);
    EXPECT_DOUBLE_EQ(matrix.get(1, 0), 6.0);
    EXPECT_DOUBLE_EQ(matrix.get(1, 1), 8.0);
}

TEST(MatrixArithmetic, MultiplicationMatchesPdf) {
    Matrix matrix(2, 2, 1.0);
    const Matrix other(2, 2, 2.0);
    matrix.mul(other);
    EXPECT_DOUBLE_EQ(matrix.get(0, 0), 4.0);
    EXPECT_DOUBLE_EQ(matrix.get(1, 1), 4.0);
    EXPECT_DOUBLE_EQ(other.get(1, 0), 2.0);
}

TEST(MatrixArithmetic, RectangularProductChangesShapeAndAccumulatesRows) {
    Matrix left(2, 3);
    Matrix right(3, 2);
    double value = 1.0;
    for (std::size_t r = 0; r < 2; ++r) {
        for (std::size_t c = 0; c < 3; ++c) left.set(r, c, value++);
    }
    value = 7.0;
    for (std::size_t r = 0; r < 3; ++r) {
        for (std::size_t c = 0; c < 2; ++c) right.set(r, c, value++);
    }
    left.mul(right);
    ASSERT_EQ(left.rows(), 2u);
    ASSERT_EQ(left.cols(), 2u);
    EXPECT_DOUBLE_EQ(left.get(0, 0), 58.0);
    EXPECT_DOUBLE_EQ(left.get(0, 1), 64.0);
    EXPECT_DOUBLE_EQ(left.get(1, 0), 139.0);
    EXPECT_DOUBLE_EQ(left.get(1, 1), 154.0);
    EXPECT_DOUBLE_EQ(right.get(2, 1), 12.0);
}

TEST(MatrixArithmetic, SelfMultiplicationUsesOriginalValues) {
    Matrix matrix = example(3.0);
    matrix.mul(matrix);
    EXPECT_DOUBLE_EQ(matrix.get(0, 0), 7.0);
    EXPECT_DOUBLE_EQ(matrix.get(0, 1), 10.0);
    EXPECT_DOUBLE_EQ(matrix.get(1, 0), 15.0);
    EXPECT_DOUBLE_EQ(matrix.get(1, 1), 22.0);
}

TEST(MatrixArithmetic, EmptyMatricesCanBeAddedAndMultiplied) {
    Matrix empty;
    empty.add(Matrix());
    empty.mul(Matrix());
    EXPECT_EQ(empty.rows(), 0u);
    EXPECT_EQ(empty.cols(), 0u);
    Matrix no_cols(2, 0);
    no_cols.add(Matrix(2, 0));
    no_cols.mul(Matrix(0, 3));
    ASSERT_EQ(no_cols.rows(), 2u);
    ASSERT_EQ(no_cols.cols(), 3u);
    EXPECT_DOUBLE_EQ(no_cols.get(1, 2), 0.0);
}

TEST(MatrixSymmetry, SymmetricPdfExample) {
    const Matrix matrix = example();
    EXPECT_TRUE(matrix.is_symmetric());
    EXPECT_DOUBLE_EQ(matrix.get(0, 1), 2.0);
}

TEST(MatrixSymmetry, NonSymmetricPdfExample) {
    EXPECT_FALSE(example(3.0).is_symmetric());
}

TEST(MatrixSymmetry, RejectsBothWideAndTallMatrices) {
    EXPECT_FALSE(Matrix(2, 3, 1.0).is_symmetric());
    EXPECT_FALSE(Matrix(3, 2, 1.0).is_symmetric());
    EXPECT_FALSE(Matrix(0, 3).is_symmetric());
    EXPECT_FALSE(Matrix(3, 0).is_symmetric());
}

TEST(MatrixSymmetry, EmptyAndSingleElementAreSymmetric) {
    EXPECT_TRUE(Matrix().is_symmetric());
    EXPECT_TRUE(Matrix(1, 1, -7.0).is_symmetric());
}

TEST(MatrixSymmetry, ChecksPairsBeyondFirstRow) {
    Matrix matrix(3, 3, 0.0);
    matrix.set(1, 2, -2.5);
    EXPECT_FALSE(matrix.is_symmetric());
    matrix.set(2, 1, -2.5);
    EXPECT_TRUE(matrix.is_symmetric());
}

TEST(MatrixSymmetry, ComparisonIsExactAsSpecified) {
    Matrix matrix = example();
    matrix.set(1, 0, 2.0 + 1e-12);
    EXPECT_FALSE(matrix.is_symmetric());
}
