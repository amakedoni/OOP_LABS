# Разбор кода лабораторной работы 3

Вариант 12. Множество хранит книги в отсортированном динамическом массиве. В каждом разделе показан код из файла и пояснение к нему.

Номера строк относятся к исходникам коммита `325d627`.

## [book.h](../book.h)

### Строки 1–6. Заголовок и начало класса

```cpp
#pragma once

#include <iostream>

class Book {
public:
```

Здесь начинается объявление книги. `#pragma once` говорит компилятору обработать этот заголовок только один раз для каждого исходника. `<iostream>` нужен, потому что ниже мы объявляем вывод книги в поток. Всё после `public:` можно вызвать снаружи класса.

### Строки 7–13. Создание, присваивание и уничтожение

```cpp
    Book() = default;
    Book(const char* title, int year);
    Book(const Book& other);
    Book(Book&& other) noexcept;
    Book& operator=(const Book& other);
    Book& operator=(Book&& other) noexcept;
    ~Book();
```

Книгу можно создать пустой, из названия и года, из другой книги или переносом из неё. Присваивание работает с уже созданной книгой. Когда книга больше не нужна, вызывается деструктор. Пять операций с ресурсом — два конструктора, два присваивания и деструктор — называют правилом пяти. `noexcept` означает, что операция перемещения не выбрасывает исключений.

### Строки 15–19. Название, сравнения и вывод

```cpp
    const char* title() const { return title_ == nullptr ? "" : title_; }

    bool operator==(const Book& other) const;
    bool operator<(const Book& other) const;
    friend std::ostream& operator<<(std::ostream& out, const Book& book);
```

`title()` возвращает адрес текста. У пустой книги адрес равен `nullptr`, поэтому метод выдаёт пустую строку, которую безопасно печатать и сравнивать. Первый `const` защищает символы от изменения, последний запрещает менять саму книгу из метода. `==` сравнивает книги, `<` определяет порядок. `friend` разрешает оператору вывода читать закрытые поля.

### Строки 21–25. Закрытые поля

```cpp
private:
    void swap(Book& other) noexcept;
    char* title_ = nullptr;
    int year_ = 0;
};
```

`title_` хранит адрес массива символов. Когда адрес равен `nullptr`, у книги нет выделенной строки. `year_` — год, по умолчанию 0. `swap` меняет поля двух книг местами. Эти детали спрятаны за `private:`, поэтому внешний код работает через методы.

## [book.cpp](../book.cpp)

### Строки 1–4. Подключения

```cpp
#include "book.h"

#include <cstring>
#include <utility>
```

Свой заголовок содержит объявления методов. `<cstring>` даёт функции для работы со строками на `char*`. Из `<utility>` здесь нужны `std::move` и `std::swap`.

### Строки 6–10. Конструктор с названием

```cpp
Book::Book(const char* title, int year) : year_(year) {
    if (title == nullptr) title = "";
    title_ = new char[std::strlen(title) + 1];
    std::strcpy(title_, title);
}
```

Сначала `year_(year)` записывает год. Если название не передали, берётся пустая строка. `strlen` считает байты текста, а `+ 1` оставляет место для знака конца строки `'\0'`. `new char[...]` выделяет место, а `strcpy` переносит туда текст вместе с завершающим нулём. Теперь книга хранит свою копию названия, даже если исходный буфер исчезнет.

### Строка 12. Конструктор копирования

```cpp
Book::Book(const Book& other) : Book(other.title(), other.year_) {}
```

Конструктор берёт название и год у другой книги и передаёт их в основной конструктор. Тот выделяет отдельный массив символов. Поэтому у двух книг может быть одинаковый текст, но при удалении одной строка второй останется жива.

### Строки 14–16. Конструктор перемещения

```cpp
Book::Book(Book&& other) noexcept {
    swap(other);
}
```

Новая книга начинается с пустого указателя и нулевого года. `swap(other)` меняет её поля с полями источника. В результате адрес строки переходит новой книге, а источник становится пустым. Сам текст остаётся по прежнему адресу.

### Строки 18–23. Копирующее присваивание

```cpp
Book& Book::operator=(const Book& other) {
    if (this == &other) return *this;
    Book copy(other);
    swap(copy);
    return *this;
}
```

`this` — адрес книги, которой присваивают значение. Если он равен `&other`, это `a = a` и можно сразу вернуть книгу. Иначе `copy` сначала создаёт независимую копию, затем `swap(copy)` отдаёт её содержимое получателю. Старое название получает временная `copy` и удаляет его при выходе. Если копия не создалась, до обмена не дойдёт.

### Строки 25–30. Перемещающее присваивание

```cpp
Book& Book::operator=(Book&& other) noexcept {
    if (this == &other) return *this;
    Book moved(std::move(other));
    swap(moved);
    return *this;
}
```

`std::move(other)` позволяет вызвать перемещающий конструктор. Сначала временная `moved` забирает строку у источника, потом `swap(moved)` передаёт её получателю. `moved` остаётся со старой строкой получателя и удаляет её в конце. Проверка адресов защищает случай, когда книга присваивается сама себе через `std::move`.

### Строки 32–37. Деструктор и обмен

```cpp
Book::~Book() { delete[] title_; }

void Book::swap(Book& other) noexcept {
    std::swap(title_, other.title_);
    std::swap(year_, other.year_);
}
```

Деструктор освобождает название тем же способом, каким оно было выделено: массиву `new[]` соответствует `delete[]`. Пустой адрес удалять безопасно. `swap` обменивает сразу адрес и год, чтобы эти поля остались данными одной книги.

### Строки 39–46. Равенство и порядок

```cpp
bool Book::operator==(const Book& other) const {
    return std::strcmp(title(), other.title()) == 0 && year_ == other.year_;
}

bool Book::operator<(const Book& other) const {
    const int order = std::strcmp(title(), other.title());
    return order < 0 || (order == 0 && year_ < other.year_);
}
```

`strcmp` сравнивает названия по байтам. Ноль означает одинаковые строки, отрицательное число ставит первое название раньше второго. При одинаковом названии порядок задаёт год. Например, `("Alpha", 2001)` идёт перед `("Alpha", 2002)`, а дубликатом первой книги будет только `("Alpha", 2001)`.

### Строки 48–50. Формат книги

```cpp
std::ostream& operator<<(std::ostream& out, const Book& book) {
    return out << "(\"" << book.title() << "\", " << book.year_ << ')';
}
```

Оператор пишет в поток название в кавычках и год, например `("Alpha", 2001)`. В строке кода `\"` означает символ кавычки. Возврат `out` позволяет сразу продолжить вывод, например `cout << book << '\n'`.

## [book_set.h](../book_set.h)

### Строки 1–14. Заголовок и правило пяти

```cpp
#pragma once

#include "book.h"

#include <cstddef>

class BookSet {
public:
    BookSet() = default;
    BookSet(const BookSet& other);
    BookSet(BookSet&& other) noexcept;
    BookSet& operator=(const BookSet& other);
    BookSet& operator=(BookSet&& other) noexcept;
    ~BookSet();
```

`BookSet` хранит элементы типа `Book`, а для размеров использует `std::size_t` из `<cstddef>`. В начале объявлены способы создать, присвоить или уничтожить множество. Они нужны отдельно, потому что класс отвечает за свою динамическую память.

### Строки 16–21. Основные методы

```cpp
    bool add(const Book& book);
    bool remove(const Book& book);
    bool contains(const Book& book) const;
    bool operator()(const Book& book) const { return contains(book); }
    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
```

`add` и `remove` возвращают `true` только тогда, когда реально изменили набор. `contains` проверяет, есть ли книга. Перегрузка `operator()` позволяет спросить то же через запись `books(book)`. Размер показывает число книг, а `empty()` проверяет, равно ли это число нулю.

### Строки 23–33. Операторы

```cpp
    BookSet& operator+=(const Book& book);
    BookSet& operator-=(const Book& book);
    BookSet operator|(const BookSet& other) const;
    BookSet operator&(const BookSet& other) const;
    BookSet operator-(const BookSet& other) const;
    bool operator==(const BookSet& other) const;
    bool operator!=(const BookSet& other) const { return !(*this == other); }

    // Только чтение: изменение книги нарушило бы порядок элементов.
    const Book& operator[](std::size_t index) const;
    friend std::ostream& operator<<(std::ostream& out, const BookSet& set);
```

`+=` и `-=` меняют то множество, к которому их применили, и возвращают ссылку на него. `|`, `&` и `-` строят новый набор. `!=` получает обратный ответ от `==`. Индекс возвращает `const Book&`, чтобы книгу можно было посмотреть без копии и чтобы замена её полей не сломала порядок.

### Строки 35–45. Внутреннее представление

```cpp
private:
    void swap(BookSet& other) noexcept;
    void reserve(std::size_t capacity);
    void append(const Book& book);
    std::size_t find_position(const Book& book) const;
    BookSet select(const BookSet& other, bool keep_common) const;

    Book* data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
};
```

`data_` — адрес массива. `size_` говорит, сколько ячеек занято, `capacity_` — сколько выделено всего. Например, при размере 3 и ёмкости 4 последняя ячейка свободна. Занятые книги всегда отсортированы и уникальны. `reserve` готовит место, `append` дописывает туда книгу, `find_position` ищет позицию, а `select` собирает нужную часть множества.

## [book_set.cpp](../book_set.cpp)

### Строки 1–10. Подключения и конструктор копирования

```cpp
#include "book_set.h"

#include <utility>

BookSet::BookSet(const BookSet& other) {
    BookSet copy;
    copy.reserve(other.size_);
    for (std::size_t i = 0; i < other.size_; ++i) copy.append(other.data_[i]);
    swap(copy);
}
```

Сначала создаём пустой временный `copy` и резервируем место ровно под число книг источника. Цикл копирует книги по порядку, поэтому сортировка сохраняется. Последний `swap` отдаёт готовый массив создаваемому множеству. Если одна из копий не создастся, временный `copy` освободит то, что уже успел выделить.

### Строки 12–28. Перемещение и два присваивания

```cpp
BookSet::BookSet(BookSet&& other) noexcept {
    swap(other);
}

BookSet& BookSet::operator=(const BookSet& other) {
    if (this == &other) return *this;
    BookSet copy(other);
    swap(copy);
    return *this;
}

BookSet& BookSet::operator=(BookSet&& other) noexcept {
    if (this == &other) return *this;
    BookSet moved(std::move(other));
    swap(moved);
    return *this;
}
```

При создании перемещением новый объект пуст, поэтому один `swap(other)` отдаёт ему массив и оставляет источник пустым. Копирующее присваивание строит локальную глубокую копию, а перемещающее — локальный `moved`, который забирает ресурс источника. После обмена временный объект удаляет старый массив получателя. Проверки адресов нужны, чтобы присваивание самому себе не трогало данные.

### Строки 30–36. Деструктор и `swap`

```cpp
BookSet::~BookSet() { delete[] data_; }

void BookSet::swap(BookSet& other) noexcept {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}
```

`delete[] data_` сначала вызывает деструктор каждой книги массива, затем освобождает ячейки. Сами строки удаляют деструкторы книг. В `swap` вместе с адресом переходят размер и ёмкость, потому что эти числа описывают именно этот массив.

### Строки 38–47. `reserve`

```cpp
void BookSet::reserve(std::size_t capacity) {
    if (capacity <= capacity_) return;
    Book* replacement = new Book[capacity];
    for (std::size_t i = 0; i < size_; ++i) {
        replacement[i] = std::move(data_[i]);
    }
    delete[] data_;
    data_ = replacement;
    capacity_ = capacity;
}
```

Если места уже хватает, функция заканчивается. Иначе создаётся новый массив пустых книг. Цикл переносит только занятые книги, а их строки остаются по прежним адресам. Затем удаляется старый массив и запоминаются новый адрес и ёмкость. Количество книг не меняется.

### Строки 49–53. `append`

```cpp
// Место для элемента заранее выделено через reserve.
void BookSet::append(const Book& book) {
    data_[size_] = book;
    ++size_;
}
```

Индекс `size_` указывает на первый свободный слот. Сначала туда копируется книга, и только после успешной копии размер растёт. Функция сама не ищет место по названию, поэтому её вызывают там, где место уже выделено и книга действительно должна встать в конец.

### Строки 55–67. Двоичный поиск позиции

```cpp
std::size_t BookSet::find_position(const Book& book) const {
    std::size_t left = 0;
    std::size_t right = size_;
    while (left < right) {
        const std::size_t middle = left + (right - left) / 2;
        if (data_[middle] < book) {
            left = middle + 1;
        } else {
            right = middle;
        }
    }
    return left;
}
```

Ищется место, где книга должна стоять. `left` входит в диапазон поиска, `right` — нет. Если книга в середине меньше нужной, игнорируем середину и всё до неё. Иначе пишем `right = middle`: продолжаем искать левее, но середина может стать окончательным ответом. Например, в `[Alpha, Beta, Delta]` для `Charlie` вернётся 2, место перед `Delta`. Значение `size_` означает вставку в конец.

### Строки 69–83. `add`

```cpp
bool BookSet::add(const Book& book) {
    const std::size_t position = find_position(book);
    if (position < size_ && data_[position] == book) return false;

    Book copy(book);
    if (size_ == capacity_) {
        reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    }
    for (std::size_t i = size_; i > position; --i) {
        data_[i] = std::move(data_[i - 1]);
    }
    data_[position] = std::move(copy);
    ++size_;
    return true;
}
```

Сначала находим позицию и проверяем, нет ли там дубликата. Копию новой книги делаем заранее, чтобы ошибка выделения памяти не застала массив наполовину перестроенным. Если места мало, расширяем массив. Для вставки `B` в `[A, C, D]` сначала передвигаем `D`, затем `C`. Так они не затирают друг друга. Остаётся поставить `B` на свободное место и увеличить размер.

### Строки 85–94. `remove`

```cpp
bool BookSet::remove(const Book& book) {
    const std::size_t position = find_position(book);
    if (position == size_ || !(data_[position] == book)) return false;
    for (std::size_t i = position; i + 1 < size_; ++i) {
        data_[i] = std::move(data_[i + 1]);
    }
    --size_;
    data_[size_] = Book();
    return true;
}
```

Сначала убеждаемся, что книга найдена. Книги справа от неё сдвигаются на одну позицию влево. Например, после удаления `B` из `[A, B, C]` книга `C` занимает её место. Уменьшаем размер и очищаем прежнюю последнюю ячейку. Если удаляли последний элемент, именно очистка освобождает его название.

### Строки 96–109. Проверка наличия и составные операторы

```cpp
bool BookSet::contains(const Book& book) const {
    const std::size_t position = find_position(book);
    return position < size_ && data_[position] == book;
}

BookSet& BookSet::operator+=(const Book& book) {
    add(book);
    return *this;
}

BookSet& BookSet::operator-=(const Book& book) {
    remove(book);
    return *this;
}
```

Двоичный поиск сообщает позицию, а `contains` отдельно проверяет, что она внутри массива и книга там совпадает. `&&` защищает от чтения за границей. `+=` и `-=` вызывают добавление или удаление и возвращают `*this` — ссылку на то же множество. Поэтому `(books += a) += b` добавляет обе книги.

### Строки 111–125. Объединение `A | B`

```cpp
BookSet BookSet::operator|(const BookSet& other) const {
    BookSet result;
    result.reserve(size_ + other.size_);
    std::size_t j = 0;
    for (std::size_t i = 0; i < size_; ++i) {
        const Book& book = data_[i];
        while (j < other.size_ && other.data_[j] < book) {
            result.append(other.data_[j++]);
        }
        result.append(book);
        if (j < other.size_ && other.data_[j] == book) ++j;
    }
    while (j < other.size_) result.append(other.data_[j++]);
    return result;
}
```

Собираем новый отсортированный массив из `A` и `B`. Перед очередной книгой `A[i]` кладём все меньшие книги из `B`, затем кладём саму `A[i]`. Если в `B` встретилась такая же книга, сдвигаем `j`, чтобы не класть её второй раз. После окончания `A` дописываем оставшийся хвост `B`. Например, `{B, D} | {A, B, E}` даёт `{A, B, D, E}`.

### Строки 127–147. Пересечение, разность и `select`

```cpp
BookSet BookSet::operator&(const BookSet& other) const {
    return select(other, true);
}

BookSet BookSet::operator-(const BookSet& other) const {
    return select(other, false);
}

BookSet BookSet::select(const BookSet& other, bool keep_common) const {
    BookSet result;
    const std::size_t max_size = keep_common && other.size_ < size_ ? other.size_ : size_;
    result.reserve(max_size);
    std::size_t j = 0;
    for (std::size_t i = 0; i < size_; ++i) {
        const Book& book = data_[i];
        while (j < other.size_ && other.data_[j] < book) ++j;
        const bool common = j < other.size_ && other.data_[j] == book;
        if (common == keep_common) result.append(book);
    }
    return result;
}
```

Оба оператора проверяют каждую книгу из `A` на совпадение с книгой из `B`. `j` пропускает в `B` всё, что меньше текущей книги, и `common` показывает, нашлась ли равная. Пересечение передаёт `true` и забирает только совпадения. Разность передаёт `false` и забирает книги без совпадения. Поэтому книга попадает в результат при `common == keep_common`. Для пересечения достаточно места под меньшее множество, для разности — под всё `A`. Например, для `A = {A, C, D}` и `B = {B, C}` пересечение даст `{C}`, а разность — `{A, D}`.

### Строки 149–167. Равенство, индекс и печать

```cpp
bool BookSet::operator==(const BookSet& other) const {
    if (size_ != other.size_) return false;
    for (std::size_t i = 0; i < size_; ++i) {
        if (!(data_[i] == other.data_[i])) return false;
    }
    return true;
}

const Book& BookSet::operator[](std::size_t index) const {
    if (index >= size_) throw "BookSet: index out of range";
    return data_[index];
}

std::ostream& operator<<(std::ostream& out, const BookSet& set) {
    if (set.empty()) return out << "{}";
    out << "{ " << set.data_[0];
    for (std::size_t i = 1; i < set.size_; ++i) out << ", " << set.data_[i];
    return out << " }";
}
```

Равенство сначала проверяет размеры, потом книги на одинаковых позициях. Сортировка гарантирует, что одинаковые наборы лежат в одном порядке. `[]` проверяет границу и возвращает ссылку только для чтения. При неверном индексе здесь бросается строка типа `const char*`. При выводе первая книга печатается сразу, а остальные с запятой перед ними.

### Сложность и пример слияния

По количеству сравнений поиск занимает `O(log n)`, вставка и удаление из-за сдвига — до `O(n)`, операции над двумя множествами — `O(n + m)`. Сравнение и копирование самих названий дополнительно зависит от длины строк.

Если `A = {Beta, Delta}` и `B = {Alpha, Beta, Epsilon}`, объединение даст `{Alpha, Beta, Delta, Epsilon}`, пересечение — `{Beta}`, а разность `A - B` — `{Delta}`.

## [main.cpp](../main.cpp)

### Строки 1–13. `read_book`

```cpp
#include "book_set.h"

bool read_book(Book& book) {
    char title[256];
    int year;
    std::cout << "Название: ";
    std::cin >> std::ws;
    if (!std::cin.getline(title, sizeof(title))) return false;
    std::cout << "Год: ";
    if (!(std::cin >> year)) return false;
    book = Book(title, year);
    return true;
}
```

Функция получает книгу, которую надо заполнить. `std::ws` убирает пробелы и перевод строки, оставшийся после прошлого ввода. `getline` читает название целиком, включая пробелы внутри. Если название слишком длинное или число года не прочиталось, возвращается `false`. При успехе создаётся книга, которая сама копирует название из локального буфера.

### Строки 15–27. `read_set`

```cpp
bool read_set(BookSet& set) {
    int count;
    std::cout << "Количество книг: ";
    if (!(std::cin >> count) || count < 0) return false;
    set = BookSet();
    for (int i = 0; i < count; ++i) {
        std::cout << "Книга " << i + 1 << ":\n";
        Book book;
        if (!read_book(book)) return false;
        set += book;
    }
    return true;
}
```

Читаем число записей. Если оно отрицательное или прочитать его не удалось, сразу возвращаем `false`. `set = BookSet()` сбрасывает прежнее множество. Цикл читает указанное число книг и добавляет каждую. Повторы пропускает `add` внутри `+=`, поэтому фактический размер может быть меньше введённого количества.

### Строки 29–31. `show`

```cpp
void show(const BookSet& set) {
    std::cout << "Множество: " << set << "\nРазмер: " << set.size() << '\n';
}
```

`show` получает множество по ссылке и печатает его через перегруженный `<<`. На второй строке показывает число уникальных книг через `size()`.

### Строки 33–44. Цикл меню и пункты 1–2

```cpp
int main() {
    BookSet books;
    int choice;
    while (true) {
        std::cout << "\n1. Создать множество\n2. Напечатать множество\n"
                     "3. Добавить книгу\n4. Удалить книгу\n5. Принадлежность\n"
                     "6. Объединение\n7. Пересечение\n8. Разность\n"
                     "9. Сравнение\n10. Книга по индексу\n0. Выход\nПункт: ";
        if (!(std::cin >> choice) || choice == 0) break;
        if (choice == 1 || choice == 2) {
            if (choice == 1 && !read_set(books)) break;
            show(books);
```

`books` создаётся один раз перед циклом, поэтому предыдущие изменения не теряются между пунктами меню. `0` или неудачное чтение завершают цикл. Пункт 1 сначала заполняет новое множество и выводит его. Пункт 2 просто показывает текущее.

### Строки 45–54. Пункты 3–5

```cpp
        } else if (choice >= 3 && choice <= 5) {
            Book book;
            if (!read_book(book)) break;
            if (choice == 5) {
                std::cout << (books(book) ? "Принадлежит: да\n" : "Принадлежит: нет\n");
                continue;
            }
            if (choice == 3) books += book;
            else books -= book;
            show(books);
```

Одна и та же книга читается для трёх действий. Пункт 5 вызывает `books(book)`, печатает ответ и через `continue` возвращается к меню. До добавления и удаления он не доходит. Пункт 3 добавляет, пункт 4 удаляет, после чего печатается текущее состояние.

### Строки 55–62. Пункты 6–9

```cpp
        } else if (choice >= 6 && choice <= 9) {
            std::cout << "Введите множество B:\n";
            BookSet other;
            if (!read_set(other)) break;
            if (choice == 6) std::cout << "A | B: " << (books | other) << '\n';
            if (choice == 7) std::cout << "A & B: " << (books & other) << '\n';
            if (choice == 8) std::cout << "A - B: " << (books - other) << '\n';
            if (choice == 9) std::cout << (books == other ? "Равны: да\n" : "Равны: нет\n");
```

В пунктах 6–9 вводится отдельное множество `B`. Для 6–8 печатается результат операции с `A`, для 9 — ответ о равенстве. Ввод и результат не заменяют `A`. Переменная `other` живёт лишь до конца этой ветки меню.

### Строки 63–76. Индекс, неизвестный пункт и выход

```cpp
        } else if (choice == 10) {
            std::size_t index;
            std::cout << "Индекс (с 0): ";
            if (!(std::cin >> index)) break;
            if (index < books.size()) {
                std::cout << "Книга[" << index << "]: " << books[index] << '\n';
            } else {
                std::cout << "Неверный индекс\n";
            }
        } else {
            std::cout << "Неизвестный пункт меню.\n";
        }
    }
}
```

Сначала читаем индекс и проверяем, существует ли такая позиция. Тогда при ошибке меню может просто вывести сообщение и продолжить работу. Сам `[]` тоже проверяет границу для любого другого вызова. Когда `main` заканчивается, деструктор `books` освобождает память.

## [test_book_set.cpp](../test_book_set.cpp)

### Строки 1–5. Заголовки и вид проверок

```cpp
#include "book_set.h"

#include <gtest/gtest.h>
#include <sstream>
#include <utility>
```

`gtest.h` подключает проверочные макросы. `sstream` позволяет собирать вывод в строку, `utility` даёт `std::move`. `EXPECT` отмечает ошибку и продолжает текущий тест. `ASSERT` останавливает его сразу, если дальше проверять уже небезопасно или бессмысленно.

### Строки 7–23. Пустое множество, добавление и индекс

```cpp
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
```

Первый тест убеждается, что новое множество пусто. Второй вставляет `Beta` перед `Alpha` и проверяет, что хранятся они в алфавитном порядке. Повторная `Alpha` должна вернуть `false`. `ASSERT_EQ` убеждается, что есть две книги, а `EXPECT_THROW` ожидает ошибку при обращении к третьей.

### Строки 25–44. Удаление и принадлежность

```cpp
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
```

Сначала проверяем, что книгу можно удалить один раз. Второе удаление возвращает `false`, поскольку её уже нет. Затем проверяем наличие двумя способами. Оба должны сказать «нет» до вставки, «да» после неё и «нет» после удаления.

### Строки 46–59. Независимость копий

```cpp
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
```

Одна копия создаётся конструктором, вторая — присваиванием готовому объекту. В первой удаляем `Alpha`, а во вторую добавляем `Beta`. Оригинал должен по-прежнему содержать только `Alpha`. Так тест проверяет, что у наборов самостоятельное содержимое.

### Строки 61–73. Перемещения

```cpp
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
```

Сначала переносим книгу из `source` при создании `moved`, потом переносим её в уже существующую `assigned`. После каждого шага прежний владелец пуст, а новый находит книгу.

### Строки 75–91. Операции над множествами

```cpp
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
```

Заранее вручную готовим ответы для двух пересекающихся множеств. `all` — всё из обоих, `common` — общая `Beta`, `difference` — `Alpha` из левого. Затем сверяем эти ответы с результатами операторов.

### Строки 93–106. Равенство

```cpp
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
```

Два набора получают одинаковые книги в разном порядке и должны считаться равными. После удаления `Beta` из одного равенство исчезает. В обоих состояниях `!=` даёт ответ, обратный `==`.

### Строки 108–127. Вывод и рост

```cpp
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
```

Проверяем точную строку вывода вместе с кавычками, пробелами и порядком книг. Затем вставляем 1000 книг с одним названием и разными годами. Они считаются разными. Повторный проход пробует вставить ровно те же книги, и размер должен остаться 1000.

## [CMakeLists.txt](../CMakeLists.txt)

### Строки 1–6. Проект и политика CMake

```cmake
cmake_minimum_required(VERSION 3.20)
project(lab03_variant12 LANGUAGES CXX)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()
```

Указываются минимальная версия CMake, имя проекта и язык C++. Если версия знает политику `CMP0135`, включается новый режим выставления времени распакованных файлов. Это помогает сборке заметить изменившийся архив.

### Строки 8–15. Google Test

```cmake
include(FetchContent)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(googletest
    URL https://codeload.github.com/google/googletest/tar.gz/refs/tags/v1.17.0
    URL_HASH SHA256=65fab701d9829d38cb77c14acdc431d2108bfdbf8979e40eb8ae567edf10b27c
)
FetchContent_MakeAvailable(googletest)
```

CMake подключает Google Test из фиксированного архива и сверяет его SHA-256. Google Mock не нужен, поэтому отключён. Вариант среды выполнения MSVC задаётся отдельной опцией для Windows. `CACHE BOOL` хранит настройки как логические значения, `FORCE` перезаписывает значение.

### Строки 17–23. Стандарт и основная программа

```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(book_set STATIC book.cpp book_set.cpp)
add_executable(lab03 main.cpp)
target_link_libraries(lab03 PRIVATE book_set)
```

Программа и тесты используют C++20 без расширений компилятора. Реализация классов собирается в библиотеку `book_set`. Программа `lab03` добавляет к ней свой `main.cpp` и получает меню.

### Строки 25–29. Отдельная программа тестов

```cmake
enable_testing()
add_executable(lab_03_tests test_book_set.cpp)
target_link_libraries(lab_03_tests PRIVATE book_set GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(lab_03_tests)
```

Тесты собираются в отдельный файл `lab_03_tests`. Они используют ту же реализацию классов, а `GTest::gtest_main` даёт им точку входа. `gtest_discover_tests` регистрирует сценарии, чтобы CTest мог запускать их по отдельности.

## [run.command](../run.command)

### Строки 1–4. Подготовка

```sh
#!/bin/sh
set -eu
cd -- "$(dirname -- "$0")"
mkdir -p build-run
```

Скрипт запускается через `/bin/sh`. `set -eu` помогает остановиться при ошибке команды или обращении к незаданной переменной. `dirname` из пути скрипта находит его папку, `cd` переходит туда. Поэтому команда компиляции видит исходники, даже если скрипт запустили из другого места.

### Строки 5–7. Компиляция и запуск

```sh
"${CXX:-c++}" -std=c++20 -Wall -Wextra -Wpedantic \
    main.cpp book.cpp book_set.cpp -o build-run/lab03
exec ./build-run/lab03
```

Берётся компилятор из `CXX` или обычный `c++`. Три исходника компилируются с C++20 и предупреждениями, `-o` задаёт имя результата. `exec` запускает только что собранную программу вместо оболочки.

## [.gitignore](../.gitignore)

### Строки 1–3. Правила игнорирования

```text
build*/
Testing/
.DS_Store
```

Git пропускает каталоги сборки, отчёты тестов и служебные файлы Finder. Это рабочие файлы, которые создаются при запуске инструментов.

## Вопросы на защите

1. **Почему копирование глубокое?** Множество выделяет свой массив, каждая книга — своё название. Деструкторы удаляют только принадлежащую им память.
2. **Что делает `std::move`?** Даёт выбрать перемещающую перегрузку. Указатель переносится при обмене полями.
3. **Почему слияние линейное?** Каждый индекс движется только вперёд. В сумме обоим нужно пройти максимум `n + m` элементов.
4. **Зачем `const Book&` у индекса?** Можно читать книгу без копии и без риска нарушить сортировку заменой элемента.
5. **Как объяснить проект в двух фразах?** Книга владеет названием, множество владеет массивом книг. Сортировка по названию и году позволяет искать двоичным поиском и сливать множества за линейное время.
