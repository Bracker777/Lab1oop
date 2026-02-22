#include "DigitString.h"
#include <algorithm>

DigitString::DigitString(std::string val) : value(val) {}

std::string DigitString::getValue() const {
    return value;
}

int DigitString::getLength() const {
    return value.length();
}

void DigitString::removeFives() {
    // Видаляємо всі символи '5'
    value.erase(std::remove(value.begin(), value.end(), '5'), value.end());
}