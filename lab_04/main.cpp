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
        } else if (choice == 4) {
            for (std::size_t i = 0; i < items.size(); ++i) {
                cout << i << ": " << items[i] << ", метрика: " << items[i].metric() << " Ом\n";
            }
            cout << "Размер: " << items.size() << ", сумма метрик: " << items.total() << '\n';
        } else if (choice >= 5 && choice <= 8) {
            std::size_t index;
            cout << "Индекс с нуля: ";
            if (!(cin >> index)) break;
            if (index >= items.size()) {
                cout << "Неверный индекс\n";
                continue;
            }

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
