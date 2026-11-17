#include "collection.h"

#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>

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

TEST(Component, DynamicCast) {
    Capacitor capacitor;
    const Component* item = &capacitor;
    EXPECT_NE(dynamic_cast<const Capacitor*>(item), nullptr);
    EXPECT_EQ(dynamic_cast<const Resistor*>(item), nullptr);
}

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

TEST(Component, ComparesMetrics) {
    Component::set_frequency(1000);
    Resistor resistor(100, 5);
    Inductor same_metric(100 / (2 * PI * 1000), true);
    EXPECT_TRUE(resistor == same_metric);
    EXPECT_TRUE(resistor == Resistor(100 + EPS / 2, 0));
    EXPECT_FALSE(resistor == Resistor(101, 5));
}

TEST(Component, ConvertsToDouble) {
    Capacitor capacitor(1e-6, false);
    const Component& item = capacitor;
    EXPECT_DOUBLE_EQ(static_cast<double>(item), item.metric());
}

TEST(Component, ChecksParameterBounds) {
    EXPECT_DOUBLE_EQ(Resistor(0, 0).metric(), 0);
    EXPECT_DOUBLE_EQ(Inductor(0, false).metric(), 0);
    EXPECT_THROW(Resistor(-1, 0), std::invalid_argument);
    EXPECT_THROW(Capacitor(0, false), std::invalid_argument);
    EXPECT_THROW(Capacitor(-1, false), std::invalid_argument);
    EXPECT_THROW(Inductor(-1, false), std::invalid_argument);
}

