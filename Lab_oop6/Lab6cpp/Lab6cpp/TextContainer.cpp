#include "TextContainer.h"
#include <algorithm>
#include <cctype>

void TextContainer::addLine(std::string text) {
    Lines.push_back(Line(text));
}

void TextContainer::removeLine(int index) {
    if (index >= 0 && index < Lines.size()) {
        Lines.erase(Lines.begin() + index);
    }
}

void TextContainer::clear() {
    Lines.clear();
}

int TextContainer::getShortestLineLength() const {
    if (Lines.empty()) return 0;

    size_t minLen = Lines[0].getData().length();
    for (const auto& line : Lines) {
        minLen = std::min(minLen, line.getData().length());
    }
    return static_cast<int>(minLen);
}

double TextContainer::getConsonantPercentage() const {
    int totalChars = 0;
    int totalConsonants = 0;

    for (const auto& line : Lines) {
        totalChars += line.getData().length();
        totalConsonants += line.countConsonants();
    }

    if (totalChars == 0) return 0.0;
    return (static_cast<double>(totalConsonants) / totalChars) * 100.0;
}

const std::vector<Line>& TextContainer::getAllLines() const {
    return Lines;
}

// Заміна сукупності пробілів, що стоять поряд, одним пробілом
std::string TextContainer::removeExtraSpaces(std::string input) {
    std::string result;
    bool lastWasSpace = false;

    for (char c : input) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!lastWasSpace) {
                result += ' ';
                lastWasSpace = true;
            }
        }
        else {
            result += c;
            lastWasSpace = false;
        }
    }
    return result;
}

// Видалення пробілів спочатку рядка та наприкінці
std::string TextContainer::trimEdges(std::string input) {
    size_t first = input.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = input.find_last_not_of(" \t\n\r");
    return input.substr(first, (last - first + 1));
}

void TextContainer::processAllLines() {
    for (size_t i = 0; i < Lines.size(); i++) {
        std::string temp = trimEdges(Lines[i].getData());
        temp = removeExtraSpaces(temp);
        Lines[i] = Line(temp);
    }
}