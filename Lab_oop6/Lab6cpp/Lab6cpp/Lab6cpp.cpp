#include <iostream>
#include <iomanip>
#include "TextContainer.h"

int main() {
    // Налаштування для коректного виведення кирилиці в консолі
    std::setlocale(LC_ALL, "");

    TextContainer myText;

    // 1. Наповнення контейнера об'єктами
    myText.addLine("   це   тестовий   рядок з   пробілами    ");
    myText.addLine(" Програмування   це   круто ");
    myText.addLine("Короткий");

    std::cout << "--- Початковий вміст контейнера ---" << std::endl;
    for (const auto& line : myText.getAllLines()) {
        std::cout << "[" << line.getData() << "]" << std::endl;
    }

    // 2. Демонстрація функціональності
    std::cout << "\nСтатистика:" << std::endl;
    std::cout << "- Довжина найкоротшого рядка: " << myText.getShortestLineLength() << std::endl;
    std::cout << "- Відсоток приголосних: " << std::fixed << std::setprecision(2)
        << myText.getConsonantPercentage() << "%" << std::endl;

    // 3. Застосування операцій над текстом
    std::cout << "\nОчищення пробілів (на початку/в кінці та зайвих всередині)..." << std::endl;
    myText.processAllLines();

    std::cout << "\n--- Результат після обробки ---" << std::endl;
    for (const auto& line : myText.getAllLines()) {
        std::cout << "[" << line.getData() << "]" << std::endl;
    }

    return 0;
}