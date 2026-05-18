#ifndef LINE_H
#define LINE_H

#include <string>

class Line {
private:
    std::string data;

public:
    Line(std::string text);
    std::string getData() const;
    int countConsonants() const; // Допоміжний метод для підрахунку приголосних
};

#endif