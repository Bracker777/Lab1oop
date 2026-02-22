using System;

namespace Lab1_Module1
{
    // Клас для роботи з рядком цифрових символів
    public class DigitString
    {
        private string _value;

        // Конструктор
        public DigitString(string value)
        {
            _value = value;
        }

        // Властивість для отримання значення
        public string Value => _value;

        // Метод обчислення довжини
        public int GetLength()
        {
            return _value.Length;
        }

        // Метод видалення символу '5'
        public void RemoveFives()
        {
            _value = _value.Replace("5", "");
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            // Створення об'єкта
            DigitString myString = new DigitString("1253456");

            // Виведення початкових даних
            Console.WriteLine($"Початковий рядок: {myString.Value}");
            Console.WriteLine($"Довжина: {myString.GetLength()}");

            // Видалення '5' та виведення результату
            myString.RemoveFives();
            Console.WriteLine($"Після видалення '5': {myString.Value}");
            Console.WriteLine($"Нова довжина: {myString.GetLength()}");
        }
    }
}