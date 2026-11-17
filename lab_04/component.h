#pragma once

#include <iostream>

constexpr double EPS = 1e-6;
constexpr double PI = 3.14159265358979323846;

class Component {
public:
    virtual ~Component() = default;
    virtual double metric() const = 0;
    virtual void print(std::ostream& out) const = 0;
    virtual void read(std::istream& in) = 0;
    virtual Component* clone() const = 0;

    explicit operator double() const { return metric(); }
    bool operator==(const Component& other) const;
    static void set_frequency(double frequency) { frequency_ = frequency; }
    static double frequency() { return frequency_; }

private:
    inline static double frequency_ = 1000.0;
};

std::ostream& operator<<(std::ostream& out, const Component& item);
std::istream& operator>>(std::istream& in, Component& item);

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
