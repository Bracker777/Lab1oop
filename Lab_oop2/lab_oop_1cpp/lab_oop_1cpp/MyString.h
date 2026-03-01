#ifndef MYSTRING_H
#define MYSTRING_H

#include <string>

class MyString {
private:
    std::string _data; 

public:
     
    MyString();
 
    MyString(std::string value);
 
    MyString(const MyString& other);

    ~MyString();

    std::string getValue() const;

    int getLength() const;

    void removeFives();
};

#endif