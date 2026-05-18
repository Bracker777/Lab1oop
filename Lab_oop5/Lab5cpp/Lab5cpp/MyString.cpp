#include "MyString.h"

// Реалізація методу базового класу
int MyString::getLength() const {
    return value.length();
}

// Реалізація для класу Digits (подвоєння кожного символу)
void Digits::expand() {
    std::string temp = "";
    for (char c : value) {
        temp += c;
        temp += c;
    }
    value = temp;
}

// Реалізація для класу Symbols (подвоєння символу 'k' або 'K')
void Symbols::expand() {
    std::string temp = "";
    for (char c : value) {
        temp += c;
        if (c == 'k' || c == 'K') {
            temp += c;
        }
    }
    value = temp;
}