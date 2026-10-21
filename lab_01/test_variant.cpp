#include "array_ops.h"

#include <gtest/gtest.h>
#include <limits>
#include <new>

namespace {
void expect_array(const int* arr, std::size_t size,
                  const int* expected, std::size_t expected_size) {
    ASSERT_EQ(size, expected_size);
    for (std::size_t i = 0; i < size; ++i) {
        EXPECT_EQ(arr[i], expected[i]) << "index = " << i;
    }
}
}

TEST(ArrayMemory, CreateAndDelete) {
    int* arr = array_create(3);
    ASSERT_NE(arr, nullptr);
    const int expected[] = {0, 0, 0};
    expect_array(arr, 3, expected, 3);
    arr[1] = 42;
    EXPECT_EQ(arr[1], 42);
    array_delete(arr);
    EXPECT_EQ(arr, nullptr);
    array_delete(arr);
    EXPECT_EQ(arr, nullptr);
}

TEST(ArrayResize, GrowPreservesPrefixAndZeroInitializesTail) {
    int* arr = array_create(2);
    arr[0] = 7;
    arr[1] = -3;
    int* original = arr;
    arr = array_resize(arr, 2, 4);
    ASSERT_NE(arr, original);
    const int expected[] = {7, -3, 0, 0};
    expect_array(arr, 4, expected, 4);
    array_delete(arr);
}

TEST(ArrayInsert, InsertInMiddle) {
    std::size_t size = 3;
    int* arr = array_create(size);
    arr[0] = 1;
    arr[1] = 3;
    arr[2] = 4;
    arr = array_insert(arr, size, 1, 2);
    const int expected[] = {1, 2, 3, 4};
    expect_array(arr, size, expected, 4);
    array_delete(arr);
}

TEST(ArrayRemove, RemoveInMiddle) {
    std::size_t size = 4;
    int* arr = array_create(size);
    for (std::size_t i = 0; i < size; ++i) arr[i] = static_cast<int>(i + 1);
    arr = array_remove(arr, size, 1);
    const int expected[] = {1, 3, 4};
    expect_array(arr, size, expected, 3);
    array_delete(arr);
}

TEST(GnomeSort, MixedNumbersAndDuplicates) {
    int arr[] = {2, -5, 2, 8, 0, 1};
    array_gnome_sort(arr, 6);
    const int expected[] = {-5, 0, 1, 2, 2, 8};
    expect_array(arr, 6, expected, 6);
}

TEST(BinarySearch, FindsFirstOccurrence) {
    const int arr[] = {-7, 0, 2, 2, 2, 8};
    std::size_t index = 99;
    ASSERT_TRUE(array_binary_search(arr, 6, 2, index));
    EXPECT_EQ(index, 2u);
}

TEST(BinarySearch, ReturnsIndexAndValueByReference) {
    const int arr[] = {-10, 0, 13};
    std::size_t index = 99;
    int value = -1;
    ASSERT_TRUE(array_binary_search(arr, 3, 13, index, value));
    EXPECT_EQ(index, 2u);
    EXPECT_EQ(value, 13);
}

TEST(RemoveAll, ExampleFromAssignmentKeepsOrder) {
    int arr[] = {2, 5, 2, 8, 2, 1};
    std::size_t size = 6;
    EXPECT_EQ(array_remove_all(arr, size, 2), 3u);
    const int expected[] = {5, 8, 1};
    expect_array(arr, size, expected, 3);
}

TEST(ArrayMemory, EmptyAndRepeatedDelete) {
    int* arr = array_create(0);
    EXPECT_EQ(arr, nullptr);
    array_delete(arr);
    array_delete(arr);
    EXPECT_EQ(arr, nullptr);
}

TEST(ArrayMemory, IndependentAllocations) {
    int* first = array_create(1);
    int* second = array_create(1);
    first[0] = 7;
    EXPECT_EQ(second[0], 0);
    array_delete(first);
    array_delete(second);
}

TEST(ArrayResize, ShrinkPreservesPrefix) {
    int* arr = array_create(4);
    for (int i = 0; i < 4; ++i) arr[i] = i + 1;
    arr = array_resize(arr, 4, 2);
    const int expected[] = {1, 2};
    expect_array(arr, 2, expected, 2);
    array_delete(arr);
}

TEST(ArrayResize, ToZeroFreesMemory) {
    int* arr = array_create(3);
    arr = array_resize(arr, 3, 0);
    EXPECT_EQ(arr, nullptr);
    array_delete(arr);
}

TEST(ArrayResize, FromEmpty) {
    int* arr = array_resize(nullptr, 0, 2);
    const int expected[] = {0, 0};
    expect_array(arr, 2, expected, 2);
    array_delete(arr);
}

TEST(ArrayResize, SameSizePreservesValues) {
    int* arr = array_create(1);
    arr[0] = 7;
    arr = array_resize(arr, 1, 1);
    EXPECT_EQ(arr[0], 7);
    array_delete(arr);
    EXPECT_EQ(array_resize(nullptr, 0, 0), nullptr);
}

TEST(ArrayResize, AllocationFailurePreservesOriginal) {
    int* arr = array_create(1);
    arr[0] = 42;
    EXPECT_THROW(array_resize(arr, 1, std::numeric_limits<std::size_t>::max()),
                 std::bad_alloc);
    EXPECT_EQ(arr[0], 42);
    array_delete(arr);
}

TEST(ArrayInsert, AtBeginningAndEnd) {
    std::size_t size = 1;
    int* arr = array_create(size);
    arr[0] = 2;
    arr = array_insert(arr, size, 0, 1);
    arr = array_insert(arr, size, size, 3);
    const int expected[] = {1, 2, 3};
    expect_array(arr, size, expected, 3);
    array_delete(arr);
}

TEST(ArrayInsert, IntoEmpty) {
    std::size_t size = 0;
    int* arr = array_insert(nullptr, size, 0, -8);
    const int expected[] = {-8};
    expect_array(arr, size, expected, 1);
    array_delete(arr);
}

TEST(ArrayInsert, InvalidPositionPreservesArray) {
    std::size_t size = 1;
    int* arr = array_create(size);
    arr[0] = 9;
    int* original = arr;
    arr = array_insert(arr, size, 2, 7);
    EXPECT_EQ(arr, original);
    const int expected[] = {9};
    expect_array(arr, size, expected, 1);
    array_delete(arr);
}

TEST(ArrayRemove, FirstAndLast) {
    std::size_t size = 4;
    int* arr = array_create(size);
    for (int i = 0; i < 4; ++i) arr[i] = i + 1;
    arr = array_remove(arr, size, 0);
    arr = array_remove(arr, size, size - 1);
    const int expected[] = {2, 3};
    expect_array(arr, size, expected, 2);
    array_delete(arr);
}

TEST(ArrayRemove, OnlyElementProducesNullptr) {
    std::size_t size = 1;
    int* arr = array_create(size);
    arr = array_remove(arr, size, 0);
    EXPECT_EQ(size, 0u);
    EXPECT_EQ(arr, nullptr);
    array_delete(arr);
}

TEST(ArrayRemove, InvalidPositionPreservesArray) {
    std::size_t size = 1;
    int* arr = array_create(size);
    arr[0] = 5;
    int* original = arr;
    arr = array_remove(arr, size, size);
    EXPECT_EQ(arr, original);
    const int expected[] = {5};
    expect_array(arr, size, expected, 1);
    array_delete(arr);
}

TEST(ArrayRemove, Empty) {
    std::size_t size = 0;
    EXPECT_EQ(array_remove(nullptr, size, 0), nullptr);
    EXPECT_EQ(size, 0u);
}

TEST(GnomeSort, EmptyAndSingleElement) {
    array_gnome_sort(nullptr, 0);
    int value = -4;
    array_gnome_sort(&value, 1);
    EXPECT_EQ(value, -4);
}

TEST(GnomeSort, ReverseOrder) {
    int arr[] = {5, 4, 3, 2, 1};
    array_gnome_sort(arr, 5);
    const int expected[] = {1, 2, 3, 4, 5};
    expect_array(arr, 5, expected, 5);
}

TEST(GnomeSort, AlreadySorted) {
    int arr[] = {-4, 0, 0, 12};
    array_gnome_sort(arr, 4);
    const int expected[] = {-4, 0, 0, 12};
    expect_array(arr, 4, expected, 4);
}

TEST(GnomeSort, ExtremeIntegers) {
    const int lo = std::numeric_limits<int>::min();
    const int hi = std::numeric_limits<int>::max();
    int arr[] = {hi, 0, lo};
    array_gnome_sort(arr, 3);
    const int expected[] = {lo, 0, hi};
    expect_array(arr, 3, expected, 3);
}

TEST(BinarySearch, FirstAndLastElements) {
    const int arr[] = {-8, 0, 4};
    std::size_t index = 99;
    ASSERT_TRUE(array_binary_search(arr, 3, -8, index));
    EXPECT_EQ(index, 0u);
    ASSERT_TRUE(array_binary_search(arr, 3, 4, index));
    EXPECT_EQ(index, 2u);
}

TEST(BinarySearch, MissingValuePreservesOutputs) {
    const int arr[] = {-8, 0, 4};
    std::size_t index = 99;
    int value = 42;
    EXPECT_FALSE(array_binary_search(arr, 3, -9, index));
    EXPECT_FALSE(array_binary_search(arr, 3, 1, index));
    EXPECT_FALSE(array_binary_search(arr, 3, 5, index, value));
    EXPECT_EQ(index, 99u);
    EXPECT_EQ(value, 42);
}

TEST(BinarySearch, EmptyPreservesOutputs) {
    std::size_t index = 99;
    int value = 42;
    EXPECT_FALSE(array_binary_search(nullptr, 0, 1, index, value));
    EXPECT_EQ(index, 99u);
    EXPECT_EQ(value, 42);
}

TEST(BinarySearch, SingleElement) {
    const int arr[] = {7};
    std::size_t index = 99;
    ASSERT_TRUE(array_binary_search(arr, 1, 7, index));
    EXPECT_EQ(index, 0u);
    EXPECT_FALSE(array_binary_search(arr, 1, 8, index));
}

TEST(RemoveAll, NoMatches) {
    int arr[] = {1, 3, 5};
    std::size_t size = 3;
    EXPECT_EQ(array_remove_all(arr, size, 2), 0u);
    const int expected[] = {1, 3, 5};
    expect_array(arr, size, expected, 3);
}

TEST(RemoveAll, EveryElementMatches) {
    int arr[] = {2, 2, 2};
    std::size_t size = 3;
    EXPECT_EQ(array_remove_all(arr, size, 2), 3u);
    EXPECT_EQ(size, 0u);
}

TEST(RemoveAll, Empty) {
    std::size_t size = 0;
    EXPECT_EQ(array_remove_all(nullptr, size, 2), 0u);
    EXPECT_EQ(size, 0u);
}

TEST(RemoveAll, AdjacentMatchesAndEnds) {
    int arr[] = {2, 2, -3, 2, 4, 2};
    std::size_t size = 6;
    EXPECT_EQ(array_remove_all(arr, size, 2), 4u);
    const int expected[] = {-3, 4};
    expect_array(arr, size, expected, 2);
}

TEST(VariantFour, SortRemoveAndSearch) {
    int arr[] = {2, 5, 2, 8, 2, 1};
    std::size_t size = 6;
    array_gnome_sort(arr, size);
    EXPECT_EQ(array_remove_all(arr, size, 2), 3u);
    const int expected[] = {1, 5, 8};
    expect_array(arr, size, expected, 3);
    std::size_t index = 99;
    int value = 0;
    ASSERT_TRUE(array_binary_search(arr, size, 5, index, value));
    EXPECT_EQ(index, 1u);
    EXPECT_EQ(value, 5);
    EXPECT_FALSE(array_binary_search(arr, size, 2, index, value));
}
