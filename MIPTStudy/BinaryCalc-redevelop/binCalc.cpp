#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cmath>

class BinaryNumber {
private:
    double value;

public:
    explicit BinaryNumber(double v = 0.0) : value(v) {}

    double toDouble() const { return value; }

    bool isInteger(double eps = 1e-12) const {
        double r = std::round(value);
        return std::fabs(value - r) < eps;
    }

    long long toIntChecked(bool &ok, double eps = 1e-12) const {
        if (!isInteger(eps)) {
            ok = false;
            return 0;
        }
        ok = true;
        return static_cast<long long>(std::llround(value));
    }

    static bool fromBinaryString(const std::string &text, BinaryNumber &out) {
        if (text.empty()) return false;

        int minusCount = 0, dotCount = 0;
        for (std::size_t i = 0; i < text.size(); ++i) {
            char c = text[i];
            if (!(c == '0' || c == '1' || c == '.' || c == '-')) return false;
            if (c == '-') {
                if (i != 0) return false;
                ++minusCount;
            } else if (c == '.') {
                ++dotCount;
            }
        }
        if (minusCount > 1 || dotCount > 1) return false;

        std::size_t start = (text[0] == '-') ? 1 : 0;
        if (start >= text.size()) return false;

        if (text[start] == '.' || text.back() == '.') return false;

        std::size_t dotPos = text.find('.');
        std::string intPartStr, fracPartStr;

        if (dotPos == std::string::npos) {
            intPartStr = text.substr(start);
        } else {
            intPartStr = text.substr(start, dotPos - start);
            fracPartStr = text.substr(dotPos + 1);
        }

        double intPart = 0.0;
        for (char c : intPartStr) {
            intPart = intPart * 2.0 + (c - '0');
        }

        double fracPart = 0.0;
        double base = 0.5;
        for (char c : fracPartStr) {
            if (c == '1') fracPart += base;
            base *= 0.5;
        }

        double result = intPart + fracPart;
        if (text[0] == '-') result = -result;

        out = BinaryNumber(result);
        return true;
    }

    std::string toBinaryString(int fracBits = 12) const {
        if (value == 0.0) return "0";

        double temp = value;
        bool neg = false;
        if (temp < 0.0) { neg = true; temp = -temp; }

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
            if (bit == 1) { fracStr.push_back('1'); fracPart -= 1.0; }
            else { fracStr.push_back('0'); }
            if (fracPart == 0.0) break;
        }

        std::string res = intStr;
        if (!fracStr.empty()) { res.push_back('.'); res += fracStr; }
        if (neg) res.insert(res.begin(), '-');
        return res;
    }
};

enum class TokenType {
    Number,
    Op,
    LParen,
    RParen,
    End
};

enum class OpKind {
    Add, Sub, Mul, Div,
    And, Or, Xor,
    UnaryMinus
};

struct Token {
    TokenType type = TokenType::End;
    BinaryNumber number;
    OpKind op = OpKind::Add;
    std::string raw;
};

class Lexer {
private:
    std::string s;
    std::size_t i = 0;

    static bool isSpace(char c) { return std::isspace(static_cast<unsigned char>(c)) != 0; }
    static bool isAlpha(char c) { return std::isalpha(static_cast<unsigned char>(c)) != 0; }

    static std::string toLower(std::string x) {
        for (char &c : x) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return x;
    }

public:
    explicit Lexer(std::string input) : s(std::move(input)) {}

    Token nextToken() {
        while (i < s.size() && isSpace(s[i])) ++i;
        if (i >= s.size()) return Token{TokenType::End, BinaryNumber(), OpKind::Add, ""};

        char c = s[i];

        if (c == '(') { ++i; return Token{TokenType::LParen, BinaryNumber(), OpKind::Add, "("}; }
        if (c == ')') { ++i; return Token{TokenType::RParen, BinaryNumber(), OpKind::Add, ")"}; }

        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '&' || c == '|' || c == '^') {
            ++i;
            Token t;
            t.type = TokenType::Op;
            t.raw = std::string(1, c);
            if (c == '+') t.op = OpKind::Add;
            if (c == '-') t.op = OpKind::Sub;
            if (c == '*') t.op = OpKind::Mul;
            if (c == '/') t.op = OpKind::Div;
            if (c == '&') t.op = OpKind::And;
            if (c == '|') t.op = OpKind::Or;
            if (c == '^') t.op = OpKind::Xor;
            return t;
        }

        if (isAlpha(c)) {
            std::size_t start = i;
            while (i < s.size() && isAlpha(s[i])) ++i;
            std::string word = toLower(s.substr(start, i - start));

            Token t;
            t.type = TokenType::Op;
            t.raw = word;

            if (word == "and") { t.op = OpKind::And; return t; }
            if (word == "or")  { t.op = OpKind::Or;  return t; }
            if (word == "xor") { t.op = OpKind::Xor; return t; }

            t.raw = word;
            t.type = TokenType::End;
            return t;
        }

        if (c == '0' || c == '1' || c == '.') {
            std::size_t start = i;
            int dotCount = 0;
            while (i < s.size()) {
                char x = s[i];
                if (x == '0' || x == '1') { ++i; continue; }
                if (x == '.') {
                    ++dotCount;
                    if (dotCount > 1) break;
                    ++i; continue;
                }
                break;
            }
            std::string numStr = s.substr(start, i - start);

            BinaryNumber bn;
            if (!BinaryNumber::fromBinaryString(numStr, bn)) {
                return Token{TokenType::End, BinaryNumber(), OpKind::Add, numStr};
            }
            Token t;
            t.type = TokenType::Number;
            t.number = bn;
            t.raw = numStr;
            return t;
        }

        return Token{TokenType::End, BinaryNumber(), OpKind::Add, std::string(1, c)};
    }
};


struct ParseResult {
    bool ok = false;
    std::string error;
    std::vector<Token> rpn;
};

static int precedence(OpKind op) {
    switch (op) {
        case OpKind::UnaryMinus: return 5;
        case OpKind::Mul:
        case OpKind::Div:        return 4;
        case OpKind::Add:
        case OpKind::Sub:        return 3;
        case OpKind::And:        return 2;
        case OpKind::Xor:        return 1;
        case OpKind::Or:         return 0;
    }
    return -1;
}

static bool isRightAssociative(OpKind op) {
    return op == OpKind::UnaryMinus;
}

class InfixParser {
public:
    static ParseResult toRpn(const std::string &expr) {
        Lexer lex(expr);
        std::vector<Token> output;
        std::vector<Token> ops;

        bool expectUnary = true;

        while (true) {
            Token t = lex.nextToken();
            if (t.type == TokenType::End) {
                if (!t.raw.empty()) {
                    return {false, "Неизвестный токен: '" + t.raw + "'", {}};
                }
                break;
            }

            if (t.type == TokenType::Number) {
                output.push_back(t);
                expectUnary = false;
                continue;
            }

            if (t.type == TokenType::LParen) {
                ops.push_back(t);
                expectUnary = true;
                continue;
            }

            if (t.type == TokenType::RParen) {
                bool found = false;
                while (!ops.empty()) {
                    if (ops.back().type == TokenType::LParen) {
                        ops.pop_back();
                        found = true;
                        break;
                    }
                    output.push_back(ops.back());
                    ops.pop_back();
                }
                if (!found) return {false, "Ошибка: лишняя ')'", {}};
                expectUnary = false;
                continue;
            }

            if (t.type == TokenType::Op) {
                if (t.op == OpKind::Sub && expectUnary) {
                    t.op = OpKind::UnaryMinus;
                    t.raw = "unary-";
                }

                while (!ops.empty() && ops.back().type == TokenType::Op) {
                    OpKind top = ops.back().op;
                    int pTop = precedence(top);
                    int pCur = precedence(t.op);

                    if (pTop > pCur || (pTop == pCur && !isRightAssociative(t.op))) {
                        output.push_back(ops.back());
                        ops.pop_back();
                    } else {
                        break;
                    }
                }

                ops.push_back(t);
                expectUnary = true;
                continue;
            }

            return {false, "Ошибка: неожиданный токен '" + t.raw + "'", {}};
        }

        while (!ops.empty()) {
            if (ops.back().type == TokenType::LParen) return {false, "Ошибка: не закрыта '('", {}};
            output.push_back(ops.back());
            ops.pop_back();
        }

        return {true, "", output};
    }
};

struct EvalResult {
    bool ok = false;
    std::string error;
    BinaryNumber value;
    bool isBitwiseResult = false;
};

class Evaluator {
private:
    static bool isZero(const BinaryNumber &b) {
        return std::fabs(b.toDouble()) < 1e-12;
    }

    static EvalResult applyUnary(OpKind op, const BinaryNumber &a) {
        if (op == OpKind::UnaryMinus) {
            return {true, "", BinaryNumber(-a.toDouble()), false};
        }
        return {false, "Неизвестный унарный оператор", BinaryNumber(), false};
    }

    static EvalResult applyBinary(OpKind op, const BinaryNumber &a, const BinaryNumber &b) {
        if (op == OpKind::Add) return {true, "", BinaryNumber(a.toDouble() + b.toDouble()), false};
        if (op == OpKind::Sub) return {true, "", BinaryNumber(a.toDouble() - b.toDouble()), false};
        if (op == OpKind::Mul) return {true, "", BinaryNumber(a.toDouble() * b.toDouble()), false};
        if (op == OpKind::Div) {
            if (isZero(b)) return {false, "Деление на ноль", BinaryNumber(), false};
            return {true, "", BinaryNumber(a.toDouble() / b.toDouble()), false};
        }

        if (op == OpKind::And || op == OpKind::Or || op == OpKind::Xor) {
            bool okA = false, okB = false;
            long long ia = a.toIntChecked(okA);
            long long ib = b.toIntChecked(okB);
            if (!okA || !okB) {
                return {false, "Логические операции (&, |, ^, and/or/xor) разрешены только для целых двоичных чисел (без точки).",
                        BinaryNumber(), false};
            }

            long long r = 0;
            if (op == OpKind::And) r = (ia & ib);
            if (op == OpKind::Or)  r = (ia | ib);
            if (op == OpKind::Xor) r = (ia ^ ib);

            return {true, "", BinaryNumber(static_cast<double>(r)), true};
        }

        return {false, "Неизвестный оператор", BinaryNumber(), false};
    }

public:
    static EvalResult evalRpn(const std::vector<Token> &rpn) {
        std::vector<BinaryNumber> st;
        bool lastWasBitwise = false;

        for (const Token &t : rpn) {
            if (t.type == TokenType::Number) {
                st.push_back(t.number);
                continue;
            }
            if (t.type == TokenType::Op) {
                if (t.op == OpKind::UnaryMinus) {
                    if (st.empty()) return {false, "Ошибка: унарный '-' без аргумента", BinaryNumber(), false};
                    BinaryNumber a = st.back(); st.pop_back();
                    EvalResult r = applyUnary(t.op, a);
                    if (!r.ok) return r;
                    st.push_back(r.value);
                    lastWasBitwise = false;
                } else {
                    if (st.size() < 2) return {false, "Ошибка: бинарный оператор без двух аргументов", BinaryNumber(), false};
                    BinaryNumber b = st.back(); st.pop_back();
                    BinaryNumber a = st.back(); st.pop_back();
                    EvalResult r = applyBinary(t.op, a, b);
                    if (!r.ok) return r;
                    st.push_back(r.value);
                    lastWasBitwise = r.isBitwiseResult;
                }
                continue;
            }
            return {false, "Ошибка: неожиданный токен в вычислении", BinaryNumber(), false};
        }

        if (st.size() != 1) return {false, "Ошибка: выражение не свелось к одному значению", BinaryNumber(), false};
        return {true, "", st.back(), lastWasBitwise};
    }
};

static std::vector<std::string> splitBySemicolon(const std::string &line) {
    std::vector<std::string> parts;
    std::string cur;
    for (char c : line) {
        if (c == ';') {
            parts.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    parts.push_back(cur);
    return parts;
}

static std::string trim(const std::string &s) {
    std::size_t a = 0;
    while (a < s.size() && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    std::size_t b = s.size();
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
    return s.substr(a, b - a);
}

int main() {
    std::cout << "Binary Expression Calculator (infix, with bitwise logic)\n";
    std::cout << "Числа: двоичные, можно с дробью через точку (пример: 101.01)\n";
    std::cout << "Операции: + - * /  , логика: & | ^  или слова and or xor\n";
    std::cout << "Скобки: ( )\n";
    std::cout << "Можно несколько выражений за раз через ';'\n";
    std::cout << "Выход: q\n\n";

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;

        line = trim(line);
        if (line == "q" || line == "Q") break;
        if (line.empty()) continue;

        auto parts = splitBySemicolon(line);
        for (std::size_t idx = 0; idx < parts.size(); ++idx) {
            std::string expr = trim(parts[idx]);
            if (expr.empty()) continue;

            ParseResult pr = InfixParser::toRpn(expr);
            if (!pr.ok) {
                std::cout << "Ошибка разбора: " << pr.error << "\n";
                continue;
            }

            EvalResult er = Evaluator::evalRpn(pr.rpn);
            if (!er.ok) {
                std::cout << "Ошибка вычисления: " << er.error << "\n";
                continue;
            }

            if (er.isBitwiseResult) {
                bool okInt = false;
                long long iv = er.value.toIntChecked(okInt);
                (void)iv;
                std::cout << er.value.toBinaryString(0) << "   (dec: " << er.value.toDouble() << ")\n";
            } else {
                std::cout << er.value.toBinaryString(12) << "   (dec: " << er.value.toDouble() << ")\n";
            }
        }
    }

    std::cout << "Пока!\n";
    return 0;
}
