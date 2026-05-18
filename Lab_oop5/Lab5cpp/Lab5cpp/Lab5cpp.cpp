#include <iostream>
#include <clocale> 
#include "MyString.h"

void demonstratePolymorphism(MyString* obj) {
    std::cout << "Поточний рядок: " << obj->getValue() << std::endl;
    std::cout << "Довжина: " << obj->getLength() << std::endl;
    std::cout << "Виконуємо розширення..." << std::endl;
    obj->expand();
    std::cout << "Після розширення: " << obj->getValue() << std::endl;
    std::cout << "------------------------------" << std::endl;
}

int main() {
    std::setlocale(LC_ALL, "Ukrainian");

    Digits d("45");
    Symbols s("kick");

    std::cout << "--- Testing Digits ---" << std::endl;
    demonstratePolymorphism(&d);

    std::cout << "--- Testing Symbols ---" << std::endl;
    demonstratePolymorphism(&s);

    return 0;
}