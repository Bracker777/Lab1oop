using System;

namespace StringLibrary
{
    // Базовий абстрактний клас
    public abstract class MyString
    {
        // Поле захищене від прямої зміни, але доступне для читання ззовні
        public string Value { get; protected set; }

        protected MyString(string value)
        {
            Value = value;
        }

        // Віртуальний метод обчислення довжини рядка
        public virtual int GetLength() => Value.Length;

        // Абстрактний метод для розширення рядка (реалізація різна для нащадків)
        public abstract void Expand();
    }

    // Похідний клас "Цифри"
    public class Digits : MyString
    {
        public Digits(string value) : base(value) { }

        // Подвоєння кожного символу
        public override void Expand()
        {
            string newStr = "";
            foreach (char c in Value)
            {
                newStr += $"{c}{c}";
            }
            Value = newStr;
        }
    }

    // Похідний клас "Символи"
    public class Symbols : MyString
    {
        public Symbols(string value) : base(value) { }

        // Подвоєння символу 'k' або 'K'
        public override void Expand()
        {
            string newStr = "";
            foreach (char c in Value)
            {
                newStr += c;
                if (c == 'k' || c == 'K')
                {
                    newStr += c; // Додаємо ще один такий самий символ
                }
            }
            Value = newStr;
        }
    }
}