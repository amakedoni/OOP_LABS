# Разбор кода лабораторной работы 4

Вариант 6, электроника. Коллекция хранит резисторы, конденсаторы и катушки через общий базовый тип. В каждом разделе показан код из файла и пояснение к нему.

Номера строк относятся к исходникам коммита `325d627`.

## [component.h](../component.h)

### Строки 1–6. Заголовок и константы

```cpp
#pragma once

#include <iostream>

constexpr double EPS = 1e-6;
constexpr double PI = 3.14159265358979323846;
```

Заголовок подключает типы потоков и защищён от повторного включения. `EPS` — допустимая разница при сравнении двух метрик. `PI` нужна для вычисления угловой частоты `2πf` у конденсатора и катушки.

### Строки 8–14. Абстрактный базовый класс

```cpp
class Component {
public:
    virtual ~Component() = default;
    virtual double metric() const = 0;
    virtual void print(std::ostream& out) const = 0;
    virtual void read(std::istream& in) = 0;
    virtual Component* clone() const = 0;
```

`Component` описывает то, что умеют все приборы. `= 0` означает, что реализация метода нужна в наследнике. Сам `Component` создать нельзя. Вызов через базовую ссылку выбирает метод фактического прибора. Виртуальный деструктор нужен, чтобы корректно удалить весь прибор даже через указатель `Component*`.

### Строки 16–23. Преобразование, равенство и частота

```cpp
    explicit operator double() const { return metric(); }
    bool operator==(const Component& other) const;
    static void set_frequency(double frequency) { frequency_ = frequency; }
    static double frequency() { return frequency_; }

private:
    inline static double frequency_ = 1000.0;
};
```

`static_cast<double>(item)` вызывает оператор преобразования, который возвращает метрику. `explicit` требует явно написать преобразование. Частота общая для всех приборов благодаря `static`, а `inline` позволяет хранить определение прямо в заголовке. Если частоту сменить, уже созданные приборы при следующем расчёте возьмут новое значение.

### Строки 25–26. Потоковые операторы

```cpp
std::ostream& operator<<(std::ostream& out, const Component& item);
std::istream& operator>>(std::istream& in, Component& item);
```

Слева у `<<` и `>>` стоит поток, поэтому это отдельные функции. Вывод читает прибор через `const&`. Ввод получает обычную ссылку и может изменить поля. Оба возвращают поток, чтобы работали цепочки вроде `in >> a >> b`.

### Строки 28–40. `Resistor`

```cpp
class Resistor : public Component {
public:
    Resistor(double resistance = 0, double tolerance = 0);
    double metric() const override { return resistance_; }
    void print(std::ostream& out) const override;
    void read(std::istream& in) override;
    Component* clone() const override { return new Resistor(*this); }
    double tolerance() const { return tolerance_; }

private:
    double resistance_;
    double tolerance_;
};
```

У резистора есть сопротивление и допуск. `metric()` возвращает сопротивление, а `tolerance()` — допуск в процентах. `clone()` через `new Resistor(*this)` создаёт независимую копию и возвращает её адрес в общем виде `Component*`. `override` помогает компилятору проверить, что метод совпадает с объявлением в базе.

### Строки 42–54. `Capacitor`

```cpp
class Capacitor : public Component {
public:
    Capacitor(double capacitance = 1e-6, bool polar = false);
    double metric() const override;
    void print(std::ostream& out) const override;
    void read(std::istream& in) override;
    Component* clone() const override { return new Capacitor(*this); }
    bool isElectrolytic() const { return polar_; }

private:
    double capacitance_;
    bool polar_;
};
```

У конденсатора есть ёмкость и логический признак `polar`. Начальная ёмкость `1e-6` Ф допустима и нужна, чтобы объект можно было создать перед вводом. `isElectrolytic()` возвращает этот признак. `clone()` создаёт копию с теми же полями и оставляет её конденсатором.

### Строки 56–68. `Inductor`

```cpp
class Inductor : public Component {
public:
    Inductor(double inductance = 0, bool air_core = false);
    double metric() const override;
    void print(std::ostream& out) const override;
    void read(std::istream& in) override;
    Component* clone() const override { return new Inductor(*this); }
    bool isAirCore() const { return air_core_; }

private:
    double inductance_;
    bool air_core_;
};
```

Катушка хранит индуктивность и признак воздушного сердечника. По умолчанию индуктивность 0, сердечник не воздушный. `isAirCore()` читает признак. `clone()` использует конструктор копирования катушки, поэтому в коллекцию попадает именно катушка.

## [component.cpp](../component.cpp)

### Строки 1–6. Заголовки

```cpp
#include "component.h"

#include <cmath>
#include <stdexcept>

namespace {
```

Первый заголовок связывает определения с классами. `<cmath>` даёт проверку конечности числа и модуль разности. `<stdexcept>` нужен для ошибки неверных аргументов. Анонимное пространство имён делает вспомогательные функции видимыми только в этом исходнике.

### Строки 8–18. Допустимые значения

```cpp
bool nonnegative(double value) {
    return std::isfinite(value) && value >= 0;
}

bool valid_capacitance(double value) {
    return std::isfinite(value) && value > 0;
}

bool valid_resistor(double resistance, double tolerance) {
    return nonnegative(resistance) && tolerance >= 0 && tolerance <= 100;
}
```

Для сопротивления и индуктивности годятся конечные числа от нуля. Ёмкость должна быть строго положительной, потому что в формуле на неё делят. Допуск резистора берём в процентах от 0 до 100. Одни и те же проверки используются при создании прибора и при чтении его полей.

### Строки 20–29. `read_label`, чтение подписи поля

```cpp
std::istream& read_label(std::istream& in, bool labelled, const char* label) {
    if (!labelled) return in;
    in >> std::ws;
    for (; *label && in; ++label) {
        if (in.get() != *label) in.setstate(std::ios::failbit);
    }
    return in;
}

}
```

Если ввод содержит только числа, функция возвращает поток сразу. Если формат с подписями, она убирает пробелы перед следующей подписью и сверяет её символ за символом. Например, ожидается `tol=` и пришло `tol:` — устанавливается `failbit`. Возврат потока позволяет сразу после корректной подписи считать число.

### Строки 31–43. Равенство и потоковые операторы

```cpp
bool Component::operator==(const Component& other) const {
    return std::abs(metric() - other.metric()) <= EPS;
}

std::ostream& operator<<(std::ostream& out, const Component& item) {
    item.print(out);
    return out;
}

std::istream& operator>>(std::istream& in, Component& item) {
    item.read(in);
    return in;
}
```

`==` берёт метрики обоих приборов, вычисляет разницу без знака и сравнивает с `EPS`. Поэтому резистор и катушка с метриками почти 100 Ом могут быть равны. `<<` и `>>` передают работу виртуальным `print` и `read`, так что одна функция годится для всех трёх типов.

### Строки 45–54. Конструктор и печать резистора

```cpp
Resistor::Resistor(double resistance, double tolerance)
    : resistance_(resistance), tolerance_(tolerance) {
    if (!valid_resistor(resistance, tolerance)) {
        throw std::invalid_argument("Invalid resistor parameters");
    }
}

void Resistor::print(std::ostream& out) const {
    out << "Resistor: R=" << resistance_ << " tol=" << tolerance_ << '%';
}
```

Список после двоеточия записывает сопротивление и допуск в поля до проверки. Если параметры выходят за допустимые границы, конструктор выбрасывает `invalid_argument`. `print` оформляет уже готовый прибор, например `Resistor: R=100 tol=5%`.

### Строки 56–68. Чтение резистора

```cpp
void Resistor::read(std::istream& in) {
    double resistance, tolerance;
    const bool labelled = (in >> std::ws).peek() == 'R';
    read_label(in, labelled, "Resistor: R=") >> resistance;
    read_label(in, labelled, "tol=") >> tolerance;
    read_label(in, labelled, "%");
    if (!in || !valid_resistor(resistance, tolerance)) {
        in.setstate(std::ios::failbit);
        return;
    }
    resistance_ = resistance;
    tolerance_ = tolerance;
}
```

Функция принимает `100 5` или `Resistor: R=100 tol=5%`. `peek()` только смотрит первый символ после пробелов. Если это `R`, следующие вызовы `read_label` проверяют подписи и знак процента. Числа пока лежат во временных переменных. Если чтение не удалось или числа не прошли проверку, прежние значения сохраняются. Только после успеха числа переходят в поля.

### Строки 70–83. Конструктор, метрика и печать конденсатора

```cpp
Capacitor::Capacitor(double capacitance, bool polar)
    : capacitance_(capacitance), polar_(polar) {
    if (!valid_capacitance(capacitance)) {
        throw std::invalid_argument("Capacitance must be positive");
    }
}

double Capacitor::metric() const {
    return (1.0 / (2 * PI)) / (frequency() * capacitance_);
}

void Capacitor::print(std::ostream& out) const {
    out << "Capacitor: C=" << capacitance_ << " F polar=" << static_cast<int>(polar_);
}
```

Конструктор проверяет ёмкость, поскольку ноль сделал бы деление невозможным. Метрика здесь равна `1/(2πfC)`: при 1000 Гц и `1e-6` Ф получается около 159,15 Ом. В подписи `F` означает фарады. Преобразование признака к `int` явно печатает 0 или 1.

### Строки 85–97. Чтение конденсатора

```cpp
void Capacitor::read(std::istream& in) {
    double capacitance;
    bool polar;
    const bool labelled = (in >> std::ws).peek() == 'C';
    read_label(in, labelled, "Capacitor: C=") >> capacitance;
    read_label(in, labelled, "F polar=") >> polar;
    if (!in || !valid_capacitance(capacitance)) {
        in.setstate(std::ios::failbit);
        return;
    }
    capacitance_ = capacitance;
    polar_ = polar;
}
```

Если первый символ `C`, читаем подписанный формат, иначе — два поля без подписей. Например, `1e-6 1` означает один микрофарад и истинный `polar`. Пока ввод не прошёл и ёмкость не проверена, поля объекта не меняются.

### Строки 99–112. Конструктор, метрика и печать катушки

```cpp
Inductor::Inductor(double inductance, bool air_core)
    : inductance_(inductance), air_core_(air_core) {
    if (!nonnegative(inductance)) {
        throw std::invalid_argument("Inductance must be nonnegative");
    }
}

double Inductor::metric() const {
    return 2 * PI * (frequency() * inductance_);
}

void Inductor::print(std::ostream& out) const {
    out << "Inductor: L=" << inductance_ << " H air_core=" << static_cast<int>(air_core_);
}
```

Конструктор разрешает нулевую индуктивность, но проверяет, что значение конечное и неотрицательное. Метрика `2πfL` растёт вместе с частотой и индуктивностью. Например, при 1000 Гц и `0.01` Гн получаем около 62,83 Ом. В подписи `H` означает генри.

### Строки 114–126. Чтение катушки

```cpp
void Inductor::read(std::istream& in) {
    double inductance;
    bool air_core;
    const bool labelled = (in >> std::ws).peek() == 'I';
    read_label(in, labelled, "Inductor: L=") >> inductance;
    read_label(in, labelled, "H air_core=") >> air_core;
    if (!in || !nonnegative(inductance)) {
        in.setstate(std::ios::failbit);
        return;
    }
    inductance_ = inductance;
    air_core_ = air_core;
}
```

Первый символ `I` включает проверку подписей. Иначе читаются только число и признак, например `0.01 0`. После успешной проверки индуктивность и признак записываются в объект. Если данные неверны, поток получает `failbit`, а объект сохраняет прежние поля.

## [collection.h](../collection.h)

### Строки 1–13. Заголовок и управление ресурсами

```cpp
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
```

Коллекция хранит динамическую память, поэтому сама описывает копирование, перемещение, оба присваивания и удаление. По умолчанию она пуста. Перемещение обменивает адрес и размеры, поэтому для него не нужно выделять новый массив.

### Строки 15–19. Общедоступные операции

```cpp
    void add(const Component& item);
    void remove(std::size_t index);
    std::size_t size() const { return size_; }
    double total() const;
    const Component& operator[](std::size_t index) const;
```

`add` берёт прибор и хранит его копию. `remove` удаляет по позиции, `total` складывает метрики. `[]` возвращает ссылку на существующий прибор. Например, выданная базовая ссылка на катушку по-прежнему вызывает формулу катушки. Копирования объекта по значению здесь нет.

### Строки 21–27. Закрытые поля и два уровня владения

```cpp
private:
    void swap(Collection& other) noexcept;
    void reserve(std::size_t capacity);
    Component** data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
};
```

Здесь два уровня. `data_` указывает на массив ячеек с адресами, а каждая занятая ячейка указывает на отдельный прибор. `size_` — сколько приборов уже есть, `capacity_` — сколько адресов помещается. Коллекция отвечает за удаление и приборов, и массива.

## [collection.cpp](../collection.cpp)

### Строки 1–11. Подключения и глубокое копирование

```cpp
#include "collection.h"

#include <stdexcept>
#include <utility>

Collection::Collection(const Collection& other) {
    Collection copy;
    copy.reserve(other.size_);
    for (std::size_t i = 0; i < other.size_; ++i) copy.add(*other.data_[i]);
    swap(copy);
}
```

Временная `copy` сначала выделяет ячейки под адреса. Цикл берёт каждый прибор по адресу и передаёт его в `add`. Внутри `add` вызывается виртуальный `clone`, поэтому копия катушки остаётся катушкой. После заполнения `swap` отдаёт массив новому объекту. Если одна копия не создастся, деструктор `copy` уберёт уже созданные приборы.

### Строки 13–29. Перемещение и присваивания

```cpp
Collection::Collection(Collection&& other) noexcept {
    swap(other);
}

Collection& Collection::operator=(const Collection& other) {
    if (this == &other) return *this;
    Collection copy(other);
    swap(copy);
    return *this;
}

Collection& Collection::operator=(Collection&& other) noexcept {
    if (this == &other) return *this;
    Collection moved(std::move(other));
    swap(moved);
    return *this;
}
```

При перемещающем создании пустая коллекция получает массив источника через обмен. При присваивании сначала готовится временная копия или временный получатель ресурса. Затем временный объект обменивается с целью и при выходе удаляет её прежние приборы. Если присваивать коллекцию самой себе, проверка адресов остановит операцию раньше.

### Строки 31–40. Деструктор и обмен

```cpp
Collection::~Collection() {
    for (std::size_t i = 0; i < size_; ++i) delete data_[i];
    delete[] data_;
}

void Collection::swap(Collection& other) noexcept {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}
```

Есть две разные памяти. `delete data_[i]` уничтожает сам прибор, а `delete[] data_` освобождает весь ряд ячеек с адресами. Виртуальный деструктор нужен для первого удаления. `swap` меняет адрес массива, его текущий размер и вместимость вместе.

### Строки 42–49. Увеличение вместимости

```cpp
void Collection::reserve(std::size_t capacity) {
    if (capacity <= capacity_) return;
    Component** replacement = new Component*[capacity];
    for (std::size_t i = 0; i < size_; ++i) replacement[i] = data_[i];
    delete[] data_;
    data_ = replacement;
    capacity_ = capacity;
}
```

Если вместимости достаточно, ничего не меняется. Иначе создаём новый ряд ячеек и копируем в него существующие адреса. Старые ячейки удаляем, а приборы остаются по тем же адресам. Поэтому ссылка на прибор может пережить расширение массива.

### Строки 51–55. Добавление

```cpp
void Collection::add(const Component& item) {
    if (size_ == capacity_) reserve(capacity_ == 0 ? 1 : capacity_ * 2);
    data_[size_] = item.clone();
    ++size_;
}
```

Если все ячейки заняты, при первом добавлении выделяется одна, а дальше ёмкость удваивается. `item.clone()` создаёт собственный прибор коллекции. Его адрес записывается в первую свободную ячейку. Только потом размер растёт, поэтому неудачное клонирование не считается добавлением.

### Строки 57–63. Удаление

```cpp
void Collection::remove(std::size_t index) {
    if (index >= size_) throw std::out_of_range("Invalid index");
    delete data_[index];
    for (std::size_t i = index + 1; i < size_; ++i) data_[i - 1] = data_[i];
    --size_;
    data_[size_] = nullptr;
}
```

Сначала проверяем, существует ли индекс. Затем удаляем прибор и сдвигаем следующие адреса. Например, при удалении конденсатора из `[резистор, конденсатор, катушка]` катушка переходит с позиции 2 на 1. Сама катушка остаётся на месте в памяти. Уменьшаем размер и очищаем лишний адрес в конце.

### Строки 65–74. Суммарная метрика и доступ по индексу

```cpp
double Collection::total() const {
    double result = 0;
    for (std::size_t i = 0; i < size_; ++i) result += data_[i]->metric();
    return result;
}

const Component& Collection::operator[](std::size_t index) const {
    if (index >= size_) throw std::out_of_range("Invalid index");
    return *data_[index];
}
```

`total` начинает с нуля и прибавляет результат `metric()` для каждого прибора. Это сумма метрик по заданию. `[]` проверяет индекс, берёт адрес из ячейки и разыменовывает его, чтобы вернуть ссылку. Эта ссылка годится, пока прибор не удалили.

## [main.cpp](../main.cpp)

### Строки 1–14. Подключение потоков и особые методы

```cpp
#include "collection.h"

using std::cin;
using std::cout;

void show_specific(const Component& item) {
    if (const auto* resistor = dynamic_cast<const Resistor*>(&item)) {
        cout << "Допуск: " << resistor->tolerance() << "%\n";
    } else if (const auto* capacitor = dynamic_cast<const Capacitor*>(&item)) {
        cout << "Электролитический: " << capacitor->isElectrolytic() << '\n';
    } else if (const auto* inductor = dynamic_cast<const Inductor*>(&item)) {
        cout << "Воздушный сердечник: " << inductor->isAirCore() << '\n';
    }
}
```

Обычные `cin` и `cout` подключаются без повторов `std::`. В `show_specific` дана только базовая ссылка. `dynamic_cast` выясняет, находится ли по этому адресу нужный наследник. Если да, указатель можно использовать для особого метода. Если тип другой, указатель пуст и программа попробует следующую ветку.

### Строки 16–29. Частота, коллекция и объекты для ввода

```cpp
int main() {
    double frequency;
    cout << "Частота, Гц: ";
    if (!(cin >> frequency) || frequency <= 0) {
        cout << "Частота должна быть положительной.\n";
        return 1;
    }
    Component::set_frequency(frequency);

    Collection items;
    Resistor resistor;
    Capacitor capacitor;
    Inductor inductor;
    Component* const inputs[] = {&resistor, &capacitor, &inductor};
```

Сначала читаем частоту и выходим с ошибкой, если она не прочиталась или неположительна. Затем создаём коллекцию и три заготовки для чтения. `inputs` хранит их адреса в порядке пунктов меню. `Component* const` не даёт переназначить адрес в ячейке, но разрешает записать новые поля в сам прибор.

### Строки 30–42. Меню и пункты 1–3

```cpp
    while (true) {
        cout << "\n1 — добавить резистор (R tol)\n2 — добавить конденсатор (C polar)\n"
                "3 — добавить катушку (L air_core)\n4 — список и сумма метрик\n"
                "5 — особый метод\n6 — удалить\n7 — сравнить\n"
                "8 — привести к double\n0 — выход\nВыбор: ";
        int choice;
        if (!(cin >> choice) || choice == 0) break;

        if (choice >= 1 && choice <= 3) {
            Component& item = *inputs[choice - 1];
            cout << "Введите поля прибора: ";
            if (!(cin >> item)) break;
            items.add(item);
```

Цикл заканчивается при `0` или ошибке чтения пункта. В пунктах 1–3 из номера вычитаем 1 и получаем индекс заготовки. `cin >> item` читает её через общий тип, но срабатывает метод нужного прибора. `items.add(item)` сохраняет копию. Например, ввод второго резистора не меняет первый, даже если читаем их в одну заготовку.

### Строки 43–47. Пункт 4, список

```cpp
        } else if (choice == 4) {
            for (std::size_t i = 0; i < items.size(); ++i) {
                cout << i << ": " << items[i] << ", метрика: " << items[i].metric() << " Ом\n";
            }
            cout << "Размер: " << items.size() << ", сумма метрик: " << items.total() << '\n';
```

По каждому индексу берём ссылку на прибор. `<<` печатает его поля, `metric()` — число в омах. После списка выводим число приборов и сумму их метрик.

### Строки 48–55. Общий ввод индекса для пунктов 5–8

```cpp
        } else if (choice >= 5 && choice <= 8) {
            std::size_t index;
            cout << "Индекс с нуля: ";
            if (!(cin >> index)) break;
            if (index >= items.size()) {
                cout << "Неверный индекс\n";
                continue;
            }
```

Действия 5–8 начинаются с ввода одного индекса. Если число не прочиталось, `break` закрывает меню. Если оно вне списка, `continue` возвращает к следующему показу меню. Так можно исправить индекс без выхода из программы.

### Строки 57–77. Действия 5–8 и выход

```cpp
            if (choice == 5) {
                show_specific(items[index]);
            } else if (choice == 6) {
                items.remove(index);
            } else if (choice == 7) {
                std::size_t second;
                cout << "Второй индекс: ";
                if (!(cin >> second)) break;
                if (second < items.size()) {
                    cout << "Метрики равны: " << (items[index] == items[second]) << '\n';
                } else {
                    cout << "Неверный индекс\n";
                }
            } else {
                cout << "double: " << static_cast<double>(items[index]) << '\n';
            }
        } else {
            cout << "Нет такого пункта\n";
        }
    }
}
```

Пункт 5 показывает особое поле через `show_specific`, 6 удаляет прибор. Пункт 7 дополнительно проверяет второй индекс и сравнивает метрики. Пункт 8 вызывает явное преобразование к `double`. Неизвестный номер сообщает об ошибке и оставляет меню открытым. При завершении коллекция уничтожает сохранённые приборы.

## [test_electronics.cpp](../test_electronics.cpp)

### Строки 1–5. Подключения и проверки Google Test

```cpp
#include "collection.h"

#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>
```

Заголовок Google Test даёт проверки, `sstream` позволяет читать приборы из заранее заданной строки, а `stdexcept` содержит тип проверяемого исключения. `EXPECT` продолжает тест после ошибки, `ASSERT` останавливает его. `EXPECT_NEAR` проверяет, что отклонение не больше указанного допуска.

### Строки 7–25. Метрики и методы трёх приборов

```cpp
TEST(Resistor, MetricAndTolerance) {
    Resistor item(100, 5);
    EXPECT_DOUBLE_EQ(item.metric(), 100);
    EXPECT_DOUBLE_EQ(item.tolerance(), 5);
}

TEST(Capacitor, MetricAndPolarFlag) {
    Component::set_frequency(1000);
    Capacitor item(1e-6, true);
    EXPECT_NEAR(item.metric(), 159.154943, EPS);
    EXPECT_TRUE(item.isElectrolytic());
}

TEST(Inductor, MetricAndCoreFlag) {
    Component::set_frequency(1000);
    Inductor item(0.01, true);
    EXPECT_NEAR(item.metric(), 62.831853, EPS);
    EXPECT_TRUE(item.isAirCore());
}
```

Проверяем каждый тип отдельно. У резистора метрика должна совпасть с сопротивлением, допуск проверяется своим методом. У конденсатора и катушки ставим частоту 1000 Гц, сравниваем результаты формул с ожидаемыми числами и проверяем их логические признаки.

### Строки 27–32. `DynamicCast`

```cpp
TEST(Component, DynamicCast) {
    Capacitor capacitor;
    const Component* item = &capacitor;
    EXPECT_NE(dynamic_cast<const Capacitor*>(item), nullptr);
    EXPECT_EQ(dynamic_cast<const Resistor*>(item), nullptr);
}
```

В переменной `item` хранится адрес конденсатора с общим типом `Component*`. Приведение к своему типу должно вернуть адрес. Попытка выдать его за резистор должна вернуть `nullptr`.

### Строки 34–48. `TotalAndRemoval`

```cpp
TEST(Collection, TotalAndRemoval) {
    Component::set_frequency(1000);
    Collection items;
    EXPECT_EQ(items.size(), 0u);
    EXPECT_DOUBLE_EQ(items.total(), 0);

    items.add(Resistor(100, 5));
    items.add(Capacitor(1e-6, true));
    items.add(Inductor(0.01, false));
    EXPECT_NEAR(items.total(), 321.986796, EPS);

    items.remove(1);
    EXPECT_EQ(items.size(), 2u);
    EXPECT_NEAR(items.total(), 162.831853, EPS);
}
```

Сначала у коллекции нет объектов и сумма равна нулю. Затем добавляем приборы всех трёх типов и сверяем сумму. После удаления элемента с индексом 1, конденсатора, должны остаться два прибора. Сумма уменьшается на его метрику.

### Строки 50–62. `ReadsFields`

```cpp
TEST(Component, ReadsFields) {
    Component::set_frequency(1000);
    Resistor resistor;
    Capacitor capacitor;
    Inductor inductor;
    std::istringstream in("100 5 1e-6 1 0.01 0");
    ASSERT_TRUE(in >> resistor >> capacitor >> inductor);
    EXPECT_DOUBLE_EQ(resistor.metric(), 100);
    EXPECT_NEAR(capacitor.metric(), 159.154943, EPS);
    EXPECT_NEAR(inductor.metric(), 62.831853, EPS);
    EXPECT_TRUE(capacitor.isElectrolytic());
    EXPECT_FALSE(inductor.isAirCore());
}
```

`istringstream` выдаёт данные как будто из консоли. Один вызов с тремя `>>` последовательно заполняет резистор, конденсатор и катушку. `ASSERT_TRUE` останавливает тест при ошибке ввода. Если всё прочиталось, сверяем метрики и два логических поля.

### Строки 64–80. `PrintsAndReadsNameAndFields`

```cpp
TEST(Component, PrintsAndReadsNameAndFields) {
    Resistor resistor;
    Capacitor capacitor;
    Inductor inductor;
    std::stringstream stream;
    stream << Resistor(100, 5) << '\n'
           << Capacitor(0.001, true) << '\n'
           << Inductor(0.01, true);
    EXPECT_EQ(stream.str(),
              "Resistor: R=100 tol=5%\n"
              "Capacitor: C=0.001 F polar=1\n"
              "Inductor: L=0.01 H air_core=1");
    ASSERT_TRUE(stream >> resistor >> capacitor >> inductor);
    EXPECT_DOUBLE_EQ(resistor.tolerance(), 5);
    EXPECT_TRUE(capacitor.isElectrolytic());
    EXPECT_TRUE(inductor.isAirCore());
}
```

Записываем три прибора в строковый поток и проверяем точные подписи. Затем читаем те же строки обратно в три других объекта. Сверяем поля, чтобы убедиться: формат, который создаёт вывод, действительно принимает ввод.

### Строки 82–89. `ComparesMetrics`

```cpp
TEST(Component, ComparesMetrics) {
    Component::set_frequency(1000);
    Resistor resistor(100, 5);
    Inductor same_metric(100 / (2 * PI * 1000), true);
    EXPECT_TRUE(resistor == same_metric);
    EXPECT_TRUE(resistor == Resistor(100 + EPS / 2, 0));
    EXPECT_FALSE(resistor == Resistor(101, 5));
}
```

Индуктивность выбрана так, чтобы при 1000 Гц метрика катушки была около 100 Ом. Резистор на 100 Ом должен считаться равным ей. Прибавка `EPS / 2` остаётся в допуске, а разница до 101 Ом уже слишком большая.

### Строки 91–95. `ConvertsToDouble`

```cpp
TEST(Component, ConvertsToDouble) {
    Capacitor capacitor(1e-6, false);
    const Component& item = capacitor;
    EXPECT_DOUBLE_EQ(static_cast<double>(item), item.metric());
}
```

Проверяем преобразование через общий тип. Ссылка `item` указывает на конденсатор. `static_cast<double>(item)` должен дать ровно тот же результат, что `item.metric()`.

### Строки 97–104. `ChecksParameterBounds`

```cpp
TEST(Component, ChecksParameterBounds) {
    EXPECT_DOUBLE_EQ(Resistor(0, 0).metric(), 0);
    EXPECT_DOUBLE_EQ(Inductor(0, false).metric(), 0);
    EXPECT_THROW(Resistor(-1, 0), std::invalid_argument);
    EXPECT_THROW(Capacitor(0, false), std::invalid_argument);
    EXPECT_THROW(Capacitor(-1, false), std::invalid_argument);
    EXPECT_THROW(Inductor(-1, false), std::invalid_argument);
}
```

Резистор и катушка с нулевым значением создаются и дают нулевую метрику. Отрицательные сопротивление и индуктивность, а также нулевая или отрицательная ёмкость должны вызвать `invalid_argument`.

## [CMakeLists.txt](../CMakeLists.txt)

### Строки 1–6. Проект и политика CMake

```cmake
cmake_minimum_required(VERSION 3.20)
project(lab04_variant6 LANGUAGES CXX)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()
```

Указываем минимальную версию CMake, название проекта и язык C++. Если политика `CMP0135` доступна, выбирается новый режим времени распакованных файлов. Благодаря условию старый CMake может пропустить эту настройку.

### Строки 8–15. Подключение Google Test

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

CMake скачивает фиксированную версию Google Test и сверяет SHA-256, чтобы получить нужное содержимое. Google Mock выключен. Опция среды выполнения относится к Windows с MSVC. `CACHE BOOL` хранит переключатель в кэше, а `FORCE` обновляет его значение.

### Строки 17–22. Стандарт и предупреждения

```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()
```

Дальнейшие цели используют C++20 без расширений. Если компилятор из семейства GCC или Clang, включаются предупреждения. Настройка стоит после подключения Google Test и применяется к целям лабораторной ниже.

### Строки 24–32. Библиотека, меню и тесты

```cmake
add_library(electronics STATIC component.cpp collection.cpp)
add_executable(lab04 main.cpp)
target_link_libraries(lab04 PRIVATE electronics)

enable_testing()
add_executable(lab_04_tests test_electronics.cpp)
target_link_libraries(lab_04_tests PRIVATE electronics GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(lab_04_tests)
```

`electronics` содержит классы приборов и коллекцию. Меню и тесты собираются как две программы и используют одну библиотеку. `GTest::gtest_main` добавляет готовую точку входа тестов. `gtest_discover_tests` позволяет CTest видеть отдельные сценарии.

## [run.command](../run.command)

### Строки 1–4. Подготовка

```sh
#!/bin/sh
set -eu
cd -- "$(dirname -- "$0")"
mkdir -p build-run
```

Первая строка запускает скрипт через `sh`. `set -eu` помогает остановиться сразу после ошибки. Из пути скрипта берётся его каталог, а `cd` переходит туда, чтобы компилятор нашёл исходники. `mkdir -p` готовит папку результата.

### Строки 5–7. Компиляция и запуск

```sh
"${CXX:-c++}" -std=c++20 -Wall -Wextra -Wpedantic \
    main.cpp component.cpp collection.cpp -o build-run/lab04
exec ./build-run/lab04
```

Компилятор берётся из `CXX` или используется `c++`. Команда собирает три исходника в `build-run/lab04` с C++20. `exec` заменяет оболочку получившейся программой и открывает меню.

## [.gitignore](../.gitignore)

### Строки 1–3. Правила игнорирования

```text
build*/
Testing/
docs/~$*
```

Эти строки просят Git пропускать каталоги сборки, отчёты тестов и временные файлы Word. Они могут появляться при работе с лабораторной.

## Вопросы на защите

1. **Что здесь делает полиморфизм?** По базовой ссылке выбирается метод фактического прибора. `dynamic_cast` нужен, когда метода вообще нет в базовом интерфейсе.
2. **Зачем виртуальный деструктор?** Коллекция удаляет приборы через `Component*`. Виртуальный деструктор уничтожает объект целиком.
3. **Зачем `clone`?** По одному интерфейсу он создаёт копию правильного производного типа. Коллекция владеет этой копией.
4. **Почему частота общая?** Все приборы сравниваются в одних условиях. Метрика читает общую частоту при вычислении.
5. **Как объяснить проект в двух фразах?** Три прибора реализуют один виртуальный интерфейс, а коллекция владеет их копиями. Формулы вычисляются по типу прибора, специальные методы вызываются после `dynamic_cast`.
