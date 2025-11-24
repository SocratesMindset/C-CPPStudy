#include <iostream>
#include <cmath>

const double PI = 3.14159265358979323846;

class radians;

class degrees {
    double value_;
public:
    explicit degrees(double v = 0) : value_(v) {}

    double value() const { return value_; }

    degrees operator+(degrees other) const { return degrees(value_ + other.value_); }
    degrees operator-(degrees other) const { return degrees(value_ - other.value_); }
    degrees operator*(double k) const { return degrees(value_ * k); }
    degrees operator/(double k) const { return degrees(value_ / k); }

    bool operator==(degrees other) const { return value_ == other.value_; }
    bool operator<(degrees other) const  { return value_ < other.value_; }

    radians toRadians() const;

    degrees& operator=(const degrees&) = default;

    degrees& operator=(const radians& r);
};

class radians {
    double value_;
public:
    explicit radians(double v = 0) : value_(v) {}

    double value() const { return value_; }

    radians operator+(radians other) const { return radians(value_ + other.value_); }
    radians operator-(radians other) const { return radians(value_ - other.value_); }
    radians operator*(double k) const { return radians(value_ * k); }
    radians operator/(double k) const { return radians(value_ / k); }

    bool operator==(radians other) const { return value_ == other.value_; }
    bool operator<(radians other) const  { return value_ < other.value_; }

    degrees toDegrees() const;

    radians& operator=(const radians&) = default;
    radians& operator=(const degrees& d);
};

radians degrees::toRadians() const {
    return radians(value_ * PI / 180.0);
}

degrees radians::toDegrees() const {
    return degrees(value_ * 180.0 / PI);
}


degrees& degrees::operator=(const radians& r) {
    value_ = r.toDegrees().value();
    return *this;
}

radians& radians::operator=(const degrees& d) {
    value_ = d.toRadians().value();
    return *this;
}

degrees operator"" _deg(long double v) {
    return degrees((double)v);
}

degrees operator"" _deg(unsigned long long v) {
    return degrees((double)v);
}

radians operator"" _rad(long double v) {
    return radians((double)v);
}

radians operator"" _rad(unsigned long long v) {
    return radians((double)v);
}

int main() {
    degrees a  = 90_deg;
    degrees a2 = 90.0_deg;

    radians b  = 1.570796_rad;

    degrees d(0);
    d = b;

    radians r(0);
    r = a;

    degrees sum = a + a2;

    std::cout << "90 deg -> rad = " << a.toRadians().value() << "\n";
    std::cout << "1.57 rad -> deg = " << b.toDegrees().value() << "\n";
    std::cout << "90_deg + 90.0_deg = " << sum.value() << " deg\n";
    std::cout << "d (from radians b) = " << d.value() << " deg\n";
    std::cout << "r (from degrees a) = " << r.value() << " rad\n";

    return 0;
}
