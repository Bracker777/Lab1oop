#include "MyString.h"
#include <algorithm>

MyString::MyString() : _data("1234567890") {}

MyString::MyString(std::string value) : _data(value) {}

MyString::MyString(const MyString& other) : _data(other._data) {}

MyString::~MyString() {}

std::string MyString::getValue() const { return _data; }

int MyString::getLength() const { return (int)_data.length(); }

void MyString::removeFives() {
    _data.erase(std::remove(_data.begin(), _data.end(), '5'), _data.end());
}