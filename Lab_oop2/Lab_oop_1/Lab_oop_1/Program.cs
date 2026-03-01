using System;

namespace Lab1_Variant12
{

    public class MyString
    {
        private string _value;   
        public MyString()
        {
            _value = "15253545";
        }
        public MyString(string value)
        {
            _value = value;
        }
        public MyString(MyString other)
        {
            _value = other._value;
        }
        ~MyString() { }
        public string GetValue()
        {
            return _value;
        }
        public int GetLength()
        {
            return _value.Length;
        }
        public void RemoveFives()
        {
            _value = _value.Replace("5", "");
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            Console.OutputEncoding = System.Text.Encoding.UTF8;
    
            MyString s2 = new MyString("15253545");
            Console.WriteLine("\n(До видалення '5'): " + s2.GetValue());
            Console.WriteLine("Довжина: " + s2.GetLength());
            s2.RemoveFives();
            Console.WriteLine("(Після видалення '5'): " + s2.GetValue());
            Console.WriteLine("Нова довжина: " + s2.GetLength());
        }
    }
}