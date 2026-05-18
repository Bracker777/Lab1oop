using System;
using TextProcessingLibrary;

namespace TextConsoleUI
{
    class Program
    {
        static void Main(string[] args)
        {
            // Встановлення кодування для коректного відображення кирилиці
            Console.OutputEncoding = System.Text.Encoding.UTF8;

            // Створення об'єкта-контейнера
            TextContainer myText = new TextContainer();

            // 1. Додавання кількох текстів (рядків)
            myText.AddLine("   це   тестовий   рядок з   пробілами    ");
            myText.AddLine(" Програмування   це   круто ");
            myText.AddLine("Короткий");

            Console.WriteLine("--- Початковий стан тексту ---");
            PrintText(myText);

            // 2. Демонстрація функціональності
            Console.WriteLine($"Довжина найкоротшого рядка: {myText.GetShortestLineLength()}");
            Console.WriteLine($"Відсоток приголосних у тексті: {myText.GetConsonantPercentage():F2}%");

            // 3. Застосування операцій очищення (інтерфейс)
            Console.WriteLine("\nЗастосовуємо очищення пробілів...");
            myText.ApplyFullCleaning();

            Console.WriteLine("\n--- Текст після обробки ---");
            PrintText(myText);

            Console.WriteLine("\nНатисніть будь-яку клавішу для виходу...");
            Console.ReadKey();
        }

        static void PrintText(TextContainer container)
        {
            foreach (var line in container.AllLines)
            {
                // Використовуємо дужки [], щоб було видно видалені пробіли по боках
                Console.WriteLine($"[{line.Data}]");
            }
        }
    }
}