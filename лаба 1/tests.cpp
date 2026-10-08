#include "array_ops.h"
#include <climits>
#include <gtest/gtest.h>


TEST(ArrayCreate, InitializesElementsToZero) {
    int* arr = array_create(3);
    ASSERT_NE(arr, nullptr);
    EXPECT_EQ(arr[0], 0);
    EXPECT_EQ(arr[1], 0);
    EXPECT_EQ(arr[2], 0);
    delete[] arr;
}

TEST(ArrayCreate, EmptyArrayIsNull) {
    int* arr = array_create(0);
    EXPECT_EQ(arr, nullptr);
    delete[] arr;
}

TEST(ArrayDelete, ClearsPointerAndAllowsRepeatedDeletion) {
    int* arr = new int[2]{1, 2};
    array_delete(arr);
    EXPECT_EQ(arr, nullptr);
    array_delete(arr);
    EXPECT_EQ(arr, nullptr);
}

TEST(ArrayResize, GrowingPreservesPrefixAndZeroesNewElements) {
    int* arr = new int[2]{4, -7};
    arr = array_resize(arr, 2, 4);
    EXPECT_EQ(arr[0], 4);
    EXPECT_EQ(arr[1], -7);
    EXPECT_EQ(arr[2], 0);
    EXPECT_EQ(arr[3], 0);
    delete[] arr;
}

TEST(ArrayResize, ShrinkingPreservesPrefix) {
    int* arr = new int[4]{4, 7, 2, 9};
    arr = array_resize(arr, 4, 2);
    EXPECT_EQ(arr[0], 4);
    EXPECT_EQ(arr[1], 7);
    delete[] arr;
}

TEST(ArrayResize, ZeroSizeReleasesArray) {
    int* arr = new int[1]{5};
    arr = array_resize(arr, 1, 0);
    EXPECT_EQ(arr, nullptr);
    delete[] arr;
}

TEST(ArrayResize, GrowsEmptyArray) {
    int* arr = array_resize(nullptr, 0, 2);
    ASSERT_NE(arr, nullptr);
    EXPECT_EQ(arr[0], 0);
    EXPECT_EQ(arr[1], 0);
    delete[] arr;
}

TEST(ArrayInsert, InsertsInMiddle) {
    int* arr = new int[3]{3, 4, 1};
    std::size_t size = 3;
    arr = array_insert(arr, size, 1, 9);
    EXPECT_EQ(size, 4U);
    EXPECT_EQ(arr[0], 3);
    EXPECT_EQ(arr[1], 9);
    EXPECT_EQ(arr[2], 4);
    EXPECT_EQ(arr[3], 1);
    delete[] arr;
}

TEST(ArrayInsert, InsertsAtBeginning) {
    int* arr = new int[2]{3, 4};
    std::size_t size = 2;
    arr = array_insert(arr, size, 0, 9);
    EXPECT_EQ(size, 3U);
    EXPECT_EQ(arr[0], 9);
    EXPECT_EQ(arr[1], 3);
    EXPECT_EQ(arr[2], 4);
    delete[] arr;
}

TEST(ArrayInsert, InsertsAtEnd) {
    int* arr = new int[2]{3, 4};
    std::size_t size = 2;
    arr = array_insert(arr, size, size, 9);
    EXPECT_EQ(size, 3U);
    EXPECT_EQ(arr[0], 3);
    EXPECT_EQ(arr[1], 4);
    EXPECT_EQ(arr[2], 9);
    delete[] arr;
}

TEST(ArrayInsert, InsertsIntoEmptyArray) {
    std::size_t size = 0;
    int* arr = array_insert(nullptr, size, 0, 9);
    ASSERT_NE(arr, nullptr);
    EXPECT_EQ(size, 1U);
    EXPECT_EQ(arr[0], 9);
    delete[] arr;
}

TEST(ArrayInsert, InvalidPositionLeavesArrayUnchanged) {
    int* arr = new int[1]{8};
    int* original = arr;
    std::size_t size = 1;
    arr = array_insert(arr, size, 2, 9);
    EXPECT_EQ(arr, original);
    EXPECT_EQ(size, 1U);
    EXPECT_EQ(arr[0], 8);
    delete[] arr;
}

TEST(ArrayRemove, RemovesFromMiddle) {
    int* arr = new int[3]{3, 9, 4};
    std::size_t size = 3;
    arr = array_remove(arr, size, 1);
    EXPECT_EQ(size, 2U);
    EXPECT_EQ(arr[0], 3);
    EXPECT_EQ(arr[1], 4);
    delete[] arr;
}

TEST(ArrayRemove, RemovesFirstElement) {
    int* arr = new int[3]{3, 9, 4};
    std::size_t size = 3;
    arr = array_remove(arr, size, 0);
    EXPECT_EQ(size, 2U);
    EXPECT_EQ(arr[0], 9);
    EXPECT_EQ(arr[1], 4);
    delete[] arr;
}

TEST(ArrayRemove, RemovesLastElement) {
    int* arr = new int[3]{3, 9, 4};
    std::size_t size = 3;
    arr = array_remove(arr, size, 2);
    EXPECT_EQ(size, 2U);
    EXPECT_EQ(arr[0], 3);
    EXPECT_EQ(arr[1], 9);
    delete[] arr;
}

TEST(ArrayRemove, RemovingOnlyElementProducesEmptyArray) {
    int* arr = new int[1]{8};
    std::size_t size = 1;
    arr = array_remove(arr, size, 0);
    EXPECT_EQ(arr, nullptr);
    EXPECT_EQ(size, 0U);
    delete[] arr;
}

TEST(ArrayRemove, EmptyArrayIsSafe) {
    std::size_t size = 0;
    EXPECT_EQ(array_remove(nullptr, size, 0), nullptr);
    EXPECT_EQ(size, 0U);
}

TEST(ArrayRemove, InvalidPositionLeavesArrayUnchanged) {
    int* arr = new int[1]{8};
    int* original = arr;
    std::size_t size = 1;
    arr = array_remove(arr, size, 1);
    EXPECT_EQ(arr, original);
    EXPECT_EQ(size, 1U);
    EXPECT_EQ(arr[0], 8);
    delete[] arr;
}


TEST(GnomeSort, SortsSampleWithDuplicates) {
    int arr[]{2, 5, 2, 8, 2, 1};
    const int expected[]{1, 2, 2, 2, 5, 8};
    array_gnome_sort(arr, 6);
    for (std::size_t i = 0; i < 6; ++i) EXPECT_EQ(arr[i], expected[i]);
}

TEST(GnomeSort, SortsDescendingArray) {
    int arr[]{5, 4, 3, 2, 1};
    const int expected[]{1, 2, 3, 4, 5};
    array_gnome_sort(arr, 5);
    for (std::size_t i = 0; i < 5; ++i) EXPECT_EQ(arr[i], expected[i]);
}

TEST(GnomeSort, HandlesNegativeNumbersAndIntegerLimits) {
    int arr[]{INT_MAX, 0, -1, INT_MIN};
    const int expected[]{INT_MIN, -1, 0, INT_MAX};
    array_gnome_sort(arr, 4);
    for (std::size_t i = 0; i < 4; ++i) EXPECT_EQ(arr[i], expected[i]);
}

TEST(GnomeSort, LeavesSortedArrayUnchanged) {
    int arr[]{-2, 0, 0, 9};
    const int expected[]{-2, 0, 0, 9};
    array_gnome_sort(arr, 4);
    for (std::size_t i = 0; i < 4; ++i) EXPECT_EQ(arr[i], expected[i]);
}

TEST(GnomeSort, SingleElementIsUnchanged) {
    int arr[]{7};
    array_gnome_sort(arr, 1);
    EXPECT_EQ(arr[0], 7);
}

TEST(GnomeSort, EmptyArrayIsSafe) {
    array_gnome_sort(nullptr, 0);
    int sentinel = 7;
    array_gnome_sort(&sentinel, 0);
    EXPECT_EQ(sentinel, 7);
}

TEST(RemoveAll, AssignmentExamplePreservesOrderWithoutSorting) {
    int arr[]{2, 5, 2, 8, 2, 1};
    std::size_t size = 6;
    EXPECT_EQ(array_remove_all(arr, size, 2), 3U);
    EXPECT_EQ(size, 3U);
    EXPECT_EQ(arr[0], 5);
    EXPECT_EQ(arr[1], 8);
    EXPECT_EQ(arr[2], 1);
}

TEST(RemoveAll, RemovesEveryElement) {
    int arr[]{2, 2, 2};
    std::size_t size = 3;
    EXPECT_EQ(array_remove_all(arr, size, 2), 3U);
    EXPECT_EQ(size, 0U);
}

TEST(RemoveAll, MissingValueLeavesArrayUnchanged) {
    int arr[]{3, 1, 4};
    std::size_t size = 3;
    EXPECT_EQ(array_remove_all(arr, size, 2), 0U);
    EXPECT_EQ(size, 3U);
    EXPECT_EQ(arr[0], 3);
    EXPECT_EQ(arr[1], 1);
    EXPECT_EQ(arr[2], 4);
}

TEST(RemoveAll, RemovesSingleMatchingElement) {
    int arr[]{-2};
    std::size_t size = 1;
    EXPECT_EQ(array_remove_all(arr, size, -2), 1U);
    EXPECT_EQ(size, 0U);
}

TEST(RemoveAll, EmptyArrayIsSafe) {
    std::size_t size = 0;
    EXPECT_EQ(array_remove_all(nullptr, size, 2), 0U);
    EXPECT_EQ(size, 0U);
}

TEST(RemoveAll, SortedWorkflowMatchesVariant) {
    int arr[]{2, 5, 2, 8, 2, 1};
    std::size_t size = 6;
    array_gnome_sort(arr, size);
    EXPECT_EQ(array_remove_all(arr, size, 2), 3U);
    EXPECT_EQ(size, 3U);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 5);
    EXPECT_EQ(arr[2], 8);
}

TEST(RemoveAll, ResizeCopiesOnlyLogicalElements) {
    int* arr = new int[4]{2, 5, 2, 8};
    std::size_t size = 4;
    array_remove_all(arr, size, 2);
    arr = array_resize(arr, size, 4);
    EXPECT_EQ(arr[0], 5);
    EXPECT_EQ(arr[1], 8);
    EXPECT_EQ(arr[2], 0);
    EXPECT_EQ(arr[3], 0);
    delete[] arr;
}

TEST(BinarySearch, FindsFirstElement) {
    const int arr[]{-4, 0, 3, 8};
    std::size_t index = 99;
    EXPECT_TRUE(array_binary_search(arr, 4, -4, index));
    EXPECT_EQ(index, 0U);
}

TEST(BinarySearch, FindsLastElement) {
    const int arr[]{-4, 0, 3, 8};
    std::size_t index = 99;
    EXPECT_TRUE(array_binary_search(arr, 4, 8, index));
    EXPECT_EQ(index, 3U);
}

TEST(BinarySearch, FindsFirstOccurrenceAmongDuplicates) {
    const int arr[]{1, 2, 2, 2, 5, 8};
    std::size_t index = 99;
    EXPECT_TRUE(array_binary_search(arr, 6, 2, index));
    EXPECT_EQ(index, 1U);
}

TEST(BinarySearch, MissingValueDoesNotChangeOutParameter) {
    const int arr[]{-4, 0, 3, 8};
    for (int target : {-10, 1, 20}) {
        std::size_t index = 99;
        EXPECT_FALSE(array_binary_search(arr, 4, target, index));
        EXPECT_EQ(index, 99U);
    }
}

TEST(BinarySearch, EmptyArrayDoesNotChangeOutParameter) {
    std::size_t index = 99;
    EXPECT_FALSE(array_binary_search(nullptr, 0, 7, index));
    EXPECT_EQ(index, 99U);
}

TEST(BinarySearch, SingleElementCanBeFound) {
    const int arr[]{7};
    std::size_t index = 99;
    EXPECT_TRUE(array_binary_search(arr, 1, 7, index));
    EXPECT_EQ(index, 0U);
}
