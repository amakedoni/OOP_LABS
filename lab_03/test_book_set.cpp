#include "book_set.h"

#include <gtest/gtest.h>
#include <sstream>
#include <utility>

TEST(BookSet, Empty) {
    BookSet books;
    EXPECT_EQ(books.size(), 0u);
    EXPECT_TRUE(books.empty());
    EXPECT_FALSE(books.contains(Book("Alpha", 2001)));
}

TEST(BookSet, AddAndIndex) {
    BookSet books;
    EXPECT_TRUE(books.add(Book("Beta", 2002)));
    EXPECT_TRUE(books.add(Book("Alpha", 2001)));
    EXPECT_FALSE(books.add(Book("Alpha", 2001)));
    ASSERT_EQ(books.size(), 2u);
    EXPECT_EQ(books[0], Book("Alpha", 2001));
    EXPECT_EQ(books[1], Book("Beta", 2002));
    EXPECT_THROW(books[2], const char*);
}

TEST(BookSet, Remove) {
    Book book("Alpha", 2001);
    BookSet books;
    books += book;
    EXPECT_TRUE(books.remove(book));
    EXPECT_FALSE(books.remove(book));
    EXPECT_TRUE(books.empty());
}

TEST(BookSet, Membership) {
    Book book("Alpha", 2001);
    BookSet books;
    EXPECT_FALSE(books(book));
    books += book;
    EXPECT_TRUE(books.contains(book));
    EXPECT_TRUE(books(book));
    books -= book;
    EXPECT_FALSE(books.contains(book));
    EXPECT_FALSE(books(book));
}

TEST(BookSet, CopyIsIndependent) {
    BookSet original;
    original += Book("Alpha", 2001);

    BookSet copy(original);
    BookSet assigned;
    assigned = original;

    copy -= Book("Alpha", 2001);
    assigned += Book("Beta", 2002);
    EXPECT_EQ(original.size(), 1u);
    EXPECT_TRUE(original.contains(Book("Alpha", 2001)));
    EXPECT_FALSE(original.contains(Book("Beta", 2002)));
}

TEST(BookSet, MoveLeavesSourceEmpty) {
    Book book("Alpha", 2001);
    BookSet source;
    source += book;
    BookSet moved(std::move(source));
    EXPECT_TRUE(source.empty());
    EXPECT_TRUE(moved.contains(book));

    BookSet assigned;
    assigned = std::move(moved);
    EXPECT_TRUE(moved.empty());
    EXPECT_TRUE(assigned.contains(book));
}

TEST(BookSet, SetOperations) {
    Book alpha("Alpha", 2001), beta("Beta", 2002), gamma("Gamma", 2003);
    BookSet a, b, all, common, difference;
    a += alpha;
    a += beta;
    b += beta;
    b += gamma;
    all += alpha;
    all += beta;
    all += gamma;
    common += beta;
    difference += alpha;

    EXPECT_EQ(a | b, all);
    EXPECT_EQ(a & b, common);
    EXPECT_EQ(a - b, difference);
}

TEST(BookSet, Equality) {
    Book alpha("Alpha", 2001), beta("Beta", 2002);
    BookSet a, b;
    a += alpha;
    a += beta;
    b += beta;
    b += alpha;
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);

    b -= beta;
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(BookSet, Output) {
    BookSet books;
    books += Book("Beta", 2002);
    books += Book("Alpha", 2001);
    std::ostringstream out;
    out << books;
    EXPECT_EQ(out.str(), "{ (\"Alpha\", 2001), (\"Beta\", 2002) }");
}

TEST(BookSet, ThousandBooks) {
    BookSet books;
    for (int year = 1000; year > 0; --year) {
        ASSERT_TRUE(books.add(Book("Book", year)));
    }
    ASSERT_EQ(books.size(), 1000u);
    for (int year = 1; year <= 1000; ++year) {
        EXPECT_FALSE(books.add(Book("Book", year)));
    }
    EXPECT_EQ(books.size(), 1000u);
}
