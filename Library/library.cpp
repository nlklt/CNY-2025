#define _CRT_SECURE_NO_WARNINGS

// Library.cpp
#include <iostream>
#include <windows.h>
#include <ctime>
#include <bitset>

extern "C"
{
    // Функция вывода числа
    void __cdecl write_int(int value)
    {
        std::cout << value << std::endl;
    }

    // Функция вывода строки
    void __cdecl write_str(char* ptr)
    {
        // Настраиваем кодировку для кириллицы, если нужно
        SetConsoleOutputCP(1251);
        if (ptr == nullptr)
        {
            std::cout << "null\n";
            return;
        }
        std::cout << ptr << std::endl;
    }

    // Функция получения системного времени
    // Возвращает указатель на статическую строку (ассемблер получит адрес в EAX)
    char* __cdecl get_time()
    {
        time_t rawtime;
        struct tm* timeinfo;
        static char buffer[80]; // Статический буфер, чтобы память не очистилась после выхода

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        // Формат: ЧЧ:ММ:СС
        strftime(buffer, 80, "%H:%M:%S", timeinfo);
        return buffer;
    }

    // Функция получения системной даты
    char* __cdecl get_date()
    {
        time_t rawtime;
        struct tm* timeinfo;
        static char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        // Формат: ДД.ММ.ГГГГ
        strftime(buffer, 80, "%d.%m.%Y", timeinfo);
        return buffer;
    }
}
