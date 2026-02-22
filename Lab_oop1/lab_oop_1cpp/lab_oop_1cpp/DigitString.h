#ifndef DIGITSTRING_H
#define DIGITSTRING_H

#include <string>

class DigitString {
private:
    std::string value;

public:
    // Конструктор
    DigitString(std::string val);

    // Геттер для значення
    std::string getValue() const;

    // Метод обчислення довжини
    int getLength() const;

    // Метод видалення символу '5'
    void removeFives();
};

#endif