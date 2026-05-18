#ifndef TEXT_CONTAINER_H
#define TEXT_CONTAINER_H

#include "Line.h"
#include <vector>
#include <string>

// Інтерфейс для очищення пробілів
class ISpaceCleaner {
public:
    virtual std::string removeExtraSpaces(std::string input) = 0;
    virtual std::string trimEdges(std::string input) = 0;
    virtual ~ISpaceCleaner() {}
};

// Клас-контейнер
class TextContainer : public ISpaceCleaner {
private:
    std::vector<Line> Lines;

public:
    void addLine(std::string text);
    void removeLine(int index);
    void clear();
    int getShortestLineLength() const;
    double getConsonantPercentage() const;
    const std::vector<Line>& getAllLines() const;

    // Реалізація інтерфейсу
    std::string removeExtraSpaces(std::string input) override;
    std::string trimEdges(std::string input) override;

    void processAllLines();
};

#endif