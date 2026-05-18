#ifndef MYSTRING_H
#define MYSTRING_H

#include <string>

// Базовий клас
class MyString {
protected:
    std::string value; // Захищене поле даних

public:
    MyString(std::string val) : value(val) {}
    virtual ~MyString() {} // Віртуальний деструктор

    std::string getValue() const { return value; }
    virtual int getLength() const;
    virtual void expand() = 0; // Чисто віртуальна функція (робить клас абстрактним)
};

// Похідний клас "Цифри"
class Digits : public MyString {
public:
    Digits(std::string val) : MyString(val) {}
    void expand() override; // Перевизначення методу розширення
};

// Похідний клас "Символи"
class Symbols : public MyString {
public:
    Symbols(std::string val) : MyString(val) {}
    void expand() override; // Перевизначення методу розширення
};

#endif