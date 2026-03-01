#include <iostream>
#include <windows.h> 
#include "MyString.h"

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
 
    MyString s1;
    std::cout << "Об'єкт 1 (стандартний): " << s1.getValue() << std::endl;
    std::cout << "Довжина: " << s1.getLength() << std::endl << std::endl;

    MyString s2("15253545556575");
    std::cout << "Об'єкт 2 (початковий): " << s2.getValue() << std::endl;

    s2.removeFives();
    std::cout << "Об'єкт 2 (після видалення '5'): " << s2.getValue() << std::endl;
    std::cout << "Нова довжина: " << s2.getLength() << std::endl << std::endl;
 
    MyString s3 = s2;
    std::cout << "Об'єкт 3 (копія Об'єкта 2): " << s3.getValue() << std::endl;

    return 0;
}