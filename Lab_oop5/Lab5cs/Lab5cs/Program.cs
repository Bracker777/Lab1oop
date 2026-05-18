using StringLibrary;
using System;
using static System.Runtime.InteropServices.JavaScript.JSType;

class Program
{
    // Метод за межами класів для демонстрації поліморфізму (на "відмінно")
    static void ShowPolymorphism(MyString obj)
    {
        Console.WriteLine($"Поточний рядок: {obj.Value}");
        Console.WriteLine($"Довжина: {obj.GetLength()}");
        Console.WriteLine("Виконуємо розширення...");
        obj.Expand();
        Console.WriteLine($"Після розширення: {obj.Value}");
        Console.WriteLine(new string('-', 30));
    }

    static void Main()
    {
        Console.OutputEncoding = System.Text.Encoding.UTF8;

        // Створення об'єктів через посилання на базовий клас
        MyString d = new Digits("123");
        MyString s = new Symbols("task");

        Console.WriteLine("--- Робота з Digits ---");
        ShowPolymorphism(d);

        Console.WriteLine("--- Робота з Symbols ---");
        ShowPolymorphism(s);
    }
}