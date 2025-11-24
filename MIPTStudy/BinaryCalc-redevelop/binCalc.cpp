#include <iostream>
#include <string>
#include <cmath>

class BinaryNumber {
private:
    double value;

    static bool isValidChar(char c) {
        return c == '0' || c == '1' || c == '.' || c == '-';
    }

public:
    explicit BinaryNumber(double v = 0.0) : value(v) {}

    double toDouble() const {
        return value;
    }

    static bool fromBinaryString(const std::string &text, BinaryNumber &out) {
        if (text.empty()) {
            return false;
        }

        for (char c : text) {
            if (!isValidChar(c)) {
                return false;
            }
        }

        int minusCount = 0;
        int dotCount = 0;
        for (std::size_t i = 0; i < text.size(); ++i) {
            if (text[i] == '-') {
                if (i != 0) {
                    return false;
                }
                ++minusCount;
            } else if (text[i] == '.') {
                ++dotCount;
            }
        }

        if (minusCount > 1 || dotCount > 1) {
            return false;
        }

        std::size_t startIndex = (text[0] == '-') ? 1 : 0;
        if (startIndex >= text.size()) {
            return false;
        }

        if (text[startIndex] == '.' || text.back() == '.') {
            return false;
        }

        std::size_t dotPos = text.find('.');
        std::string intPartStr;
        std::string fracPartStr;

        if (dotPos == std::string::npos) {
            intPartStr = text.substr(startIndex);
        } else {
            intPartStr = text.substr(startIndex, dotPos - startIndex);
            fracPartStr = text.substr(dotPos + 1);
        }

        double intPart = 0.0;
        for (char c : intPartStr) {
            intPart = intPart * 2.0 + (c - '0');
        }

        double fracPart = 0.0;
        double base = 0.5;
        for (char c : fracPartStr) {
            if (c == '1') {
                fracPart += base;
            }
            base *= 0.5;
        }

        double result = intPart + fracPart;
        if (text[0] == '-') {
            result = -result;
        }

        out = BinaryNumber(result);
        return true;
    }

    std::string toBinaryString(int fracBits = 8) const {
        if (value == 0.0) {
            return "0";
        }

        double temp = value;
        bool isNegative = false;
        if (temp < 0.0) {
            isNegative = true;
            temp = -temp;
        }

        long long intPart = static_cast<long long>(temp);
        double fracPart = temp - static_cast<double>(intPart);

        std::string intStr;
        if (intPart == 0) {
            intStr = "0";
        } else {
            while (intPart > 0) {
                int bit = static_cast<int>(intPart % 2);
                intStr.insert(intStr.begin(), static_cast<char>('0' + bit));
                intPart /= 2;
            }
        }

        std::string fracStr;
        for (int i = 0; i < fracBits; ++i) {
            fracPart *= 2.0;
            int bit = static_cast<int>(fracPart);
            if (bit == 1) {
                fracStr.push_back('1');
                fracPart -= 1.0;
            } else {
                fracStr.push_back('0');
            }
            if (fracPart == 0.0) {
                break;
            }
        }

        std::string result = intStr;
        if (!fracStr.empty()) {
            result.push_back('.');
            result += fracStr;
        }

        if (isNegative) {
            result.insert(result.begin(), '-');
        }

        return result;
    }
};

class BinaryCalculator {
public:
    static BinaryNumber add(const BinaryNumber &a, const BinaryNumber &b) {
        return BinaryNumber(a.toDouble() + b.toDouble());
    }

    static BinaryNumber subtract(const BinaryNumber &a, const BinaryNumber &b) {
        return BinaryNumber(a.toDouble() - b.toDouble());
    }

    static BinaryNumber multiply(const BinaryNumber &a, const BinaryNumber &b) {
        return BinaryNumber(a.toDouble() * b.toDouble());
    }

    static bool divide(const BinaryNumber &a, const BinaryNumber &b, BinaryNumber &out) {
        double divisor = b.toDouble();
        if (std::fabs(divisor) < 1e-12) {
            return false;
        }
        out = BinaryNumber(a.toDouble() / divisor);
        return true;
    }
};

BinaryNumber readBinaryNumber(const std::string &name) {
    while (true) {
        std::cout << "Введите число " << name
                  << " в двоичном виде (например, 101.01 или -10.1): ";
        std::string text;
        if (!(std::cin >> text)) {
            std::cout << "Ошибка ввода. Попробуйте ещё раз.\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        BinaryNumber number;
        if (BinaryNumber::fromBinaryString(text, number)) {
            return number;
        }

        std::cout << "Некорректное двоичное число. Разрешены только символы 0, 1, '.', '-' и не более одной точки.\n";
    }
}

char readOperation() {
    while (true) {
        std::cout << "\nВыберите операцию (+, -, *, /) или q для выхода: ";
        char op;
        if (!(std::cin >> op)) {
            std::cout << "Ошибка ввода. Попробуйте ещё раз.\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (op == '+' || op == '-' || op == '*' || op == '/' || op == 'q' || op == 'Q') {
            return op;
        }

        std::cout << "Неизвестная операция. Введите +, -, *, / или q.\n";
    }
}

int main() {
    std::cout << "Простой двоичный калькулятор (с плавающей точкой)." << std::endl;
    std::cout << "Вводите числа в двоичном виде, дробная часть отделяется точкой." << std::endl;

    while (true) {
        char op = readOperation();
        if (op == 'q' || op == 'Q') {
            std::cout << "Выход из программы." << std::endl;
            break;
        }

        BinaryNumber a = readBinaryNumber("A");
        BinaryNumber b = readBinaryNumber("B");

        BinaryNumber result;
        bool ok = true;

        if (op == '+') {
            result = BinaryCalculator::add(a, b);
        } else if (op == '-') {
            result = BinaryCalculator::subtract(a, b);
        } else if (op == '*') {
            result = BinaryCalculator::multiply(a, b);
        } else if (op == '/') {
            ok = BinaryCalculator::divide(a, b, result);
        }

        if (!ok) {
            std::cout << "Ошибка: деление на ноль запрещено." << std::endl;
            continue;
        }

        std::cout << "Результат в двоичном виде: " << result.toBinaryString(12) << std::endl;
        std::cout << "Результат в десятичном виде: " << result.toDouble() << std::endl;
    }

    return 0;
}
b
