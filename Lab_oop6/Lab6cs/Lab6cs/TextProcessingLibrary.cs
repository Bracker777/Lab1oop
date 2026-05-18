using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;

namespace TextProcessingLibrary
{
    // Інтерфейс для видалення зайвих пробілів
    public interface ISpaceCleaner
    {
        string CleanInternalSpaces(string input);
        string CleanEdges(string input);
    }

    // Клас-рядок
    public class Line
    {
        // Обмеження прямого доступу (властивість тільки для читання)
        public string Data { get; private set; }

        public Line(string text)
        {
            Data = text;
        }

        // Метод для підрахунку приголосних літер
        public int CountConsonants()
        {
            string consonants = "bcdfghjklmnpqrstvwxyzбвгджзйклмнпрстфхцчшщ";
            return Data.ToLower().Count(c => consonants.Contains(c));
        }
    }

    // Клас-контейнер (Абстракція тексту)
    public class TextContainer : ISpaceCleaner
    {
        private List<Line> Lines = new List<Line>();

        // Властивість для доступу до рядків (тільки для читання)
        public IReadOnlyList<Line> AllLines => Lines;

        // Додавання рядка
        public void AddLine(string text)
        {
            Lines.Add(new Line(text));
        }

        // Видалення рядка
        public void RemoveLineAt(int index)
        {
            if (index >= 0 && index < Lines.Count)
            {
                Lines.RemoveAt(index);
            }
        }

        // Очищення всього тексту
        public void ClearAll()
        {
            Lines.Clear();
        }

        // Отримання довжини найкоротшого рядка
        public int GetShortestLineLength()
        {
            if (Lines.Count == 0) return 0;
            return Lines.Min(l => l.Data.Length);
        }

        // Повернення відсотка приголосних літер у всьому тексті
        public double GetConsonantPercentage()
        {
            int totalChars = Lines.Sum(l => l.Data.Length);
            if (totalChars == 0) return 0;

            int totalConsonants = Lines.Sum(l => l.CountConsonants());
            return (double)totalConsonants / totalChars * 100;
        }

        // Реалізація інтерфейсу ISpaceCleaner
        public string CleanInternalSpaces(string input)
        {
            return Regex.Replace(input, @"\s+", " ");
        }

        public string CleanEdges(string input)
        {
            return input.Trim();
        }

        // Метод для повної обробки всіх рядків у контейнері
        public void ApplyFullCleaning()
        {
            for (int i = 0; i < Lines.Count; i++)
            {
                string processed = CleanEdges(Lines[i].Data);
                processed = CleanInternalSpaces(processed);
                Lines[i] = new Line(processed); // Перестворення об'єкта
            }
        }
    }
}