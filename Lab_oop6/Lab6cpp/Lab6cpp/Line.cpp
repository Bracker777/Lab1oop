#include "Line.h"
#include <cctype>
#include <algorithm>

Line::Line(std::string test) : data(test) {}

std::string Line::getData() const {
    return data;
}

int Line::countConsonants() const {
    int count = 0;
    // Ğÿäîê ì³ñòèòü àíãë³éñüê³ òà óêğà¿íñüê³ ïğèãîëîñí³ ë³òåğè (â îáîõ ğåã³ñòğàõ)
    std::string consonants = "bcdfghjklmnpqrstvwxyzBCDFGHJKLMNPQRSTVWXYZáâãäæçéêëìíïğñòôõö÷øùÁÂÃÄÆÇÉÊËÌÍÏĞÑÒÔÕÖ×ØÙ";

    for (char c : data) {
        if (consonants.find(c) != std::string::npos) {
            count++;
        }
    }
    return count;
}