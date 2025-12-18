#include "error.h"

namespace Colors {
    void enableColors() {
#ifdef _WIN32
        // получаем дескриптор стандартного вывода
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) return;

        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode)) return;

        // включаем флаг обработки управляющих последовательностей
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
#endif
    }
}

namespace Error
{
    /*серии ошибок: 0 - 99    - системные ошибки
                    100 - 109 - ошибки параметров
                    110 - 119 - ошибки открытия и чтения файлов
                    120 - 129 - ошибки таблиц идентификаторов и лексем
                    200 - 209 - ошибки лексического анализа*/
    ERROR errors[ERROR_MAX_ENTRY] =
    {
        ERROR_ENTRY(0, "Недопустимый код ошибки"),
            ERROR_ENTRY(1, "Системный сбой"),
            ERROR_ENTRY_NODEF(2), ERROR_ENTRY_NODEF(3), ERROR_ENTRY_NODEF(4), ERROR_ENTRY_NODEF(5),
            ERROR_ENTRY_NODEF(6), ERROR_ENTRY_NODEF(7), ERROR_ENTRY_NODEF(8), ERROR_ENTRY_NODEF(9),
            ERROR_ENTRY_NODEF10(10), ERROR_ENTRY_NODEF10(20), ERROR_ENTRY_NODEF10(30), ERROR_ENTRY_NODEF10(40), 
            ERROR_ENTRY_NODEF10(50), ERROR_ENTRY_NODEF10(60), ERROR_ENTRY_NODEF10(70), ERROR_ENTRY_NODEF10(80), 
            ERROR_ENTRY_NODEF10(90),

            ERROR_ENTRY(100, "Нет аргумента после параметра -i"),
            ERROR_ENTRY(101, "Нет аргумента после параметра -l"),
            ERROR_ENTRY(102, "Нет аргумента после параметра -o"), 
            ERROR_ENTRY(103, "Параметр должен быть разделен с аргументами проболем"),
            ERROR_ENTRY(104, "Параметр -i должен быть задан"), 
            ERROR_ENTRY(105, "Превышена длина входного параметра"), 
            ERROR_ENTRY(106, "Параметр команднойстроки не распознан"),
            ERROR_ENTRY_NODEF(107), ERROR_ENTRY_NODEF(108), ERROR_ENTRY_NODEF(109),

            ERROR_ENTRY(110, "Ошибка при открытии файла с исходным кодом (-i)"),
            ERROR_ENTRY(111, "Недопустимый символ в исходном файле (-i)"),
            ERROR_ENTRY(112, "Ошибка при создании файла протокола (-l)"),
            ERROR_ENTRY_NODEF(113), ERROR_ENTRY_NODEF(114), ERROR_ENTRY_NODEF(115),
            ERROR_ENTRY_NODEF(116), ERROR_ENTRY_NODEF(117), ERROR_ENTRY_NODEF(118), ERROR_ENTRY_NODEF(119),

            ERROR_ENTRY(120, "Превышен допустимый размер при создании таблиццы идентификаторов"),
            ERROR_ENTRY(121, "Превышен допустимый размер при создании таблиццы лексем"),
            ERROR_ENTRY(122, "Переполнение таблиццы идентификаторов"),
            ERROR_ENTRY(123, "Переполнение таблиццы лексем"),
            ERROR_ENTRY(124, "Переданный индекс выходит за границы допустимых значений таблиццы идентификаторов"),
            ERROR_ENTRY(125, "Переданный индекс выходит за границы допустимых значений таблиццы лексем"),
            ERROR_ENTRY_NODEF(126), ERROR_ENTRY_NODEF(127), ERROR_ENTRY_NODEF(128), ERROR_ENTRY_NODEF(129),

            ERROR_ENTRY_NODEF10(130), ERROR_ENTRY_NODEF10(140), ERROR_ENTRY_NODEF10(150),
            ERROR_ENTRY_NODEF10(160), ERROR_ENTRY_NODEF10(170), ERROR_ENTRY_NODEF10(180), ERROR_ENTRY_NODEF10(190),

            ERROR_ENTRY(200, "Переобъявление переменной в 1 области видимости   "),
            ERROR_ENTRY(201, "Переполнение целочисленного типа                  "),
            ERROR_ENTRY(202, "Строковый литерал превышает максимальную длину    "),
            ERROR_ENTRY(203, "Несоответствующая закрывающая скобка '}'          "),
            ERROR_ENTRY(204, "Незакрытый строковый литерал                      "),
            ERROR_ENTRY(205, "Неизвестный символ                                "),
            ERROR_ENTRY(206, "Недопустимый символ в записи восьмеричного числа. "),
            ERROR_ENTRY(207, "Обнаружено несколько точек входа main.            "),
            ERROR_ENTRY(208, "Не обнаружено точки входа main.                   "),
            ERROR_ENTRY_NODEF(209),

            ERROR_ENTRY_NODEF10(210), ERROR_ENTRY_NODEF10(220), ERROR_ENTRY_NODEF10(230), ERROR_ENTRY_NODEF10(240),
            ERROR_ENTRY_NODEF10(250), ERROR_ENTRY_NODEF10(260), ERROR_ENTRY_NODEF10(270), ERROR_ENTRY_NODEF10(280),
            ERROR_ENTRY_NODEF10(290),

            ERROR_ENTRY(300, "Типы значений должны быть целыми числовыми."),
            ERROR_ENTRY(301, "Использование неопределенного идентификатора."),
            ERROR_ENTRY(302, "Несоответствие типов в выражении."),
            ERROR_ENTRY(303, "Недопустимые операторы в выражении."),
            ERROR_ENTRY(304, "Ожидались параметры для функции."),
            ERROR_ENTRY(305, "Неверный тип аргумента."),
            ERROR_ENTRY(306, "Недостаточно аргументов для функции."),
            ERROR_ENTRY(307, "Превышено число допустимых параметров функции."),
            ERROR_ENTRY(308, "Оператор return вне функции."),
            ERROR_ENTRY(309, "Тип возвращаемого значения не соответствует типу функции."),
            ERROR_ENTRY(310, "Смешение типов при присваивании."),
            ERROR_ENTRY(311, "Слишком много аргументов для функции."),
            ERROR_ENTRY(312, "Деление числа на 0."),
            ERROR_ENTRY(313, "Обязателен возврат значения в функции main."),
            
            ERROR_ENTRY_NODEF(314), ERROR_ENTRY_NODEF(315),
            ERROR_ENTRY_NODEF(316), ERROR_ENTRY_NODEF(317), ERROR_ENTRY_NODEF(318), ERROR_ENTRY_NODEF(319),
            ERROR_ENTRY_NODEF10(320), ERROR_ENTRY_NODEF10(330), ERROR_ENTRY_NODEF10(340), ERROR_ENTRY_NODEF10(350),
            ERROR_ENTRY_NODEF10(360), ERROR_ENTRY_NODEF10(370), ERROR_ENTRY_NODEF10(380), ERROR_ENTRY_NODEF10(390),
            ERROR_ENTRY_NODEF100(400), ERROR_ENTRY_NODEF100(500),
            ERROR_ENTRY(600, "Неверная структура программы"),
            ERROR_ENTRY(601, "Ошибочный оператор"),
            ERROR_ENTRY(602, "Ошибка в выражении"),
            ERROR_ENTRY(603, "Ошибка в действиях"),
            ERROR_ENTRY(604, "Ошибка в объявлении"),
            ERROR_ENTRY(605, "Ошибка в вызове"),

            ERROR_ENTRY_NODEF(606), ERROR_ENTRY_NODEF(607), ERROR_ENTRY_NODEF(608),
            ERROR_ENTRY_NODEF(609),
            ERROR_ENTRY_NODEF10(610), ERROR_ENTRY_NODEF10(620), ERROR_ENTRY_NODEF10(630), ERROR_ENTRY_NODEF10(640),
            ERROR_ENTRY_NODEF10(650), ERROR_ENTRY_NODEF10(660), ERROR_ENTRY_NODEF10(670), ERROR_ENTRY_NODEF10(680),
            ERROR_ENTRY_NODEF10(690),


            ERROR_ENTRY_NODEF100(700), ERROR_ENTRY_NODEF100(800), ERROR_ENTRY_NODEF100(900),

    };

    ERROR  geterror(int  id)
    {
        ERROR result;

        if (id < 0 || id >= ERROR_MAX_ENTRY)
        {
            result = errors[0];
        }
        else
        {
            result = errors[id];
        }

        return result;
    }
    ERROR  geterrorin(int  id, int  line = -1, int  col = -1)
    {
        ERROR result;

        if (id < 0 || id >= ERROR_MAX_ENTRY)
        {
            result = errors[0];
        }
        else
        {
            result = errors[id];

            result.inext.col = col;
            result.inext.line = line;
        }

        return result;
    }
};