#include <iostream>
#include "DigitString.h"
#include <Windows.h> // Для коректного відображення кирилиці

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    // Створення об'єкта
    DigitString myStr("985756");

    // Виведення даних 
    std::cout << "Початковий рядок: " << myStr.getValue() << std::endl;
    std::cout << "Довжина: " << myStr.getLength() << std::endl;

    // Модифікація та виведення результату
    myStr.removeFives();
    std::cout << "Після видалення '5': " << myStr.getValue() << std::endl;
    std::cout << "Нова довжина: " << myStr.getLength() << std::endl;

    return 0;
}