#include "component.h"

#include <cmath>
#include <stdexcept>

namespace {

bool nonnegative(double value) {
    return std::isfinite(value) && value >= 0;
}

bool valid_capacitance(double value) {
    return std::isfinite(value) && value > 0;
}

bool valid_resistor(double resistance, double tolerance) {
    return nonnegative(resistance) && tolerance >= 0 && tolerance <= 100;
}

std::istream& read_label(std::istream& in, bool labelled, const char* label) {
    if (!labelled) return in;
    in >> std::ws;
    for (; *label && in; ++label) {
        if (in.get() != *label) in.setstate(std::ios::failbit);
    }
    return in;
}

}

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

Resistor::Resistor(double resistance, double tolerance)
    : resistance_(resistance), tolerance_(tolerance) {
    if (!valid_resistor(resistance, tolerance)) {
        throw std::invalid_argument("Invalid resistor parameters");
    }
}

void Resistor::print(std::ostream& out) const {
    out << "Resistor: R=" << resistance_ << " tol=" << tolerance_ << '%';
}

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
