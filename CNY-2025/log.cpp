#include "log.h"

#include <iostream>
#include <fstream>
#include <cstdarg>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <clocale>
#include <cstring>
#include <map>

namespace Log {

    // Преобразование idtype/iddatatype в человекочитаемые строки
    static const char* IdTypeToString(IT::IDTYPE t)
    {
        switch (t)
        {
        case IT::V: return "V(variable)";
        case IT::F: return "F(function)";
        case IT::P: return "P(param)";
        case IT::C: return "C(call)";
        case IT::UNKNOWN: default: return "UNKNOWN";
        }
    }

    static const char* IdDataTypeToString(IT::IDDATATYPE t)
    {
        switch (t)
        {
        case IT::INT: return "INT";
        case IT::CHR: return "CHAR";
        case IT::STR: return "STRING";
        case IT::UNDEF:
        default: return "?UNDEF?";
        }
    }

    // Преобразование lexema char (символ лексемы) в строку
    static std::string LexemeToString(char lex)
    {
        switch (lex)
        {
        case LT_TYPE: return           "TYPE";
        case LT_ID: return             "ID";
        case LT_LITERAL: return        "LITERAL";
        case LT_FUNCTION: return       "FUNCTION";
        case LT_RETURN: return         "RETURN";
        case LT_FOR: return            "FOR";
        case LT_RANGE: return          "RANGE";
        case LT_MAIN: return           "MAIN";
        case LT_EQUAL: return          "EQUAL";
        case LT_UNARY: return             "UNARY";
        case LT_OPERATION: return             "OPERATION";
        case LT_COMMA: return           "COMMA";
        case LT_SEMICOLON: return      "SEMICOLON";
        case LT_LEFTBRACE: return      "LEFTBRACE";
        case LT_RIGHTBRACE: return     "RIGHTBRACE";
        case LT_LEFTHESIS: return      "LEFTHESIS";
        case LT_RIGHTHESIS: return    "RIGHTTHESIS";
        default:
            if (std::isprint(static_cast<unsigned char>(lex)))
            {
                std::string s(1, lex);
                return std::string("'") + s + "'";
            }
            else
            {
                char buf[8];
                sprintf_s(buf, "0x%02X", (unsigned char)lex);
                return std::string(buf);
            }
        }
    }

    // Преобразование mean (token mean) в строку — используем enum tokenMean из LT.h
    static const char* MeanToString(char m)
    {
        switch (m)
        {
        case LT_ID: return             "ID";
        case LT_FUNCTION: return            "FUN";
        case LT_RETURN: return         "RETURN";
        case LT_FOR: return            "FOR";
        case LT_RANGE: return          "RANGE";
        case LT_MAIN: return           "MAIN";
        case LT_EQUAL: return          "EQUAL";
        case LT_COMMA: return           "COMMA";
        case LT_SEMICOLON: return      "SEMICOLON";
        case LT_LEFTBRACE: return      "LEFTBRACE";
        case LT_RIGHTBRACE: return     "RIGHTBRACE";
        case LT_LEFTHESIS: return      "LEFTHESIS";
        case LT_RIGHTHESIS: return    "RIGHTTHESIS";

        case LT::SIGNATURE::sign: return            "sign";
        case LT::SIGNATURE::variable: return        "variable";
        case LT::SIGNATURE::function: return        "function";
        case LT::SIGNATURE::parameter: return       "parameter";
        case LT::SIGNATURE::call: return            "call";
        case LT::SIGNATURE::t_int: return             "INT";
        case LT::SIGNATURE::t_char: return            "CHAR";
        case LT::SIGNATURE::t_string: return          "STRING";
        case LT::SIGNATURE::number: return          "number";
        case LT::SIGNATURE::symbol: return          "symbol";
        case LT::SIGNATURE::string: return            "string";
        case LT::SIGNATURE::print: return           "print";
        case LT::SIGNATURE::date: return            "date";
        case LT::SIGNATURE::time: return            "time";
        case LT::SIGNATURE::increment: return       "increment";
        case LT::SIGNATURE::dicrement: return       "dicrement";
        case LT::SIGNATURE::inversion: return       "inversion";
        case LT::SIGNATURE::plus: return            "plus";
        case LT::SIGNATURE::minus: return           "minus";
        case LT::SIGNATURE::division: return        "division";
        case LT::SIGNATURE::multiplication: return  "multiplication";
        default: return             "???";
        }
    }

    LOG getlog(wchar_t logfile[])
    {
        LOG log;
        wcscpy_s(log.logfile, PARM_MAX_SIZE, logfile);

        log.stream = new std::ofstream();

        log.stream->open(logfile, std::ios::out | std::ios::trunc);
        if (!log.stream || !log.stream->is_open())
        {
            delete log.stream;
            log.stream = nullptr;
            ERROR_THROW(112);
        }

        setlocale(LC_ALL, "rus");

        return log;
    }

    void WriteLine(LOG log, char* c, ...)
    {
        if (!log.stream) return;

        va_list args;
        va_start(args, c);

        const char* s = c;
        while (s != nullptr && *s != '\0')
        {
            *log.stream << s;
            s = va_arg(args, char*);
        }

        *log.stream << std::endl;
        va_end(args);
    }

    void WriteLine(LOG log, const wchar_t* first, ...)
    {
        if (!log.stream) return;

        va_list args;
        va_start(args, first);

        const wchar_t* s = first;
        char buffer[4096];
        size_t converted = 0;

        while (s != nullptr && std::wcscmp(s, L"") != 0)
        {
            wcstombs_s(&converted, buffer, sizeof(buffer), s, _TRUNCATE);
            *log.stream << buffer;
            s = va_arg(args, const wchar_t*);
        }

        *log.stream << std::endl;
        va_end(args);
    }

    void WriteLog(LOG log)
    {
        if (!log.stream) return;

        char time_buffer[128];
        time_t now = std::time(NULL);

        tm t;
        localtime_s(&t, &now);
        strftime(time_buffer, sizeof(time_buffer), "%d.%m.%Y %H:%M:%S", &t);

        WriteLine(log, (char*)"Дата создания протокола: ", (char*)time_buffer, (char*)"", "");
    }

    void WriteParm(LOG log, Parm::PARM parm)
    {
        if (!log.stream) return;

        WriteLine(log, (char*)"\n------------- Параметры -------------", (char*)"", "");
        WriteLine(log, 
            (wchar_t*)L"-i:  ", (wchar_t*)parm.in, L"\n",
            (wchar_t*)L"-l:  ", (wchar_t*)parm.log, L"\n",
            (wchar_t*)L"-o:  ", (wchar_t*)parm.out, L""
        );
        WriteLine(log, (char*)"-------------------------------------", (char*)"", "");
    }

    void WriteIn(LOG log, In::IN in)
    {
        if (!log.stream) return;

        wchar_t size_buf[32];
        wchar_t ignor_buf[32];
        wchar_t lines_buf[32];

        _itow_s(in.size, size_buf, 10);
        _itow_s(in.ignor, ignor_buf, 10);
        _itow_s(in.lines, lines_buf, 10);

        WriteLine(log, (char*)"\n---------- Исходные данные ----------", (char*)"", "");
        WriteLine(log, 
            (wchar_t*)L"Количество символов: ", (wchar_t*)size_buf, L"\n",
            (wchar_t*)L"Проигнорировано    : ", (wchar_t*)ignor_buf, L"\n",
            (wchar_t*)L"Количество строк   : ", (wchar_t*)lines_buf, L""
        );
        WriteLine(log, (char*)"-------------------------------------", (char*)"", "");
    }

    void WriteError(LOG log, Error::ERROR error)
    {
        if (log.stream != nullptr && log.stream->is_open())
        {
            char linebuf[32] = "";
            char colbuf[32] = "";

            if (error.inext.line >= 0) _itoa_s(error.inext.line, linebuf, 10);
            if (error.inext.col >= 0) _itoa_s(error.inext.col, colbuf, 10);

            if (error.inext.line >= 0 && error.inext.col >= 0)
            {
                WriteLine(log, (char*)"Ошибка: ", (char*)error.message, (char*)"  строка: ", (char*)linebuf, (char*)"  позиция: ", (char*)colbuf, (char*)"", "");
            }
            else
            {
                WriteLine(log, (char*)"Ошибка: ", (char*)error.message, (char*)"", "");
            }
        }
        else
        {
            std::cerr << "Ошибка " << error.id << ": " << error.message;
            if (error.inext.line >= 0 && error.inext.col >= 0)
            {
                std::cerr << " (строка " << error.inext.line << ", позиция " << error.inext.col << ")";
            }
            std::cerr << std::endl;
        }
    }

    void WriteLexTable(LOG log, LT::LexTable& lt, IT::IdTable* it)
    {
        if (!log.stream) return;

        char ltsize[32];
        sprintf_s(ltsize, "%d", lt.size);

        WriteLine(log, (char*)"---------- Таблица лексем ----------", (char*)"", "");
        WriteLine(log, (char*)"Всего лексем: ", (char*)ltsize, (char*)"", "");

        // Заголовок таблицы
        const int W_IDX = 6;
        const int W_LEX = 18;
        const int W_mean = 20;
        const int W_LINE = 8;
        const int W_IT = 4;

        const int W_INDEX = 8;
        const int W_NAME = 21;
        const int W_SCOPE = 15;
        const int W_TYPE = 15;
        const int W_DATATYPE = 9;
        const int W_VALUE = 16;

        *log.stream << std::left
            << std::setw(W_IDX) << "LT"
            << std::setw(W_LEX) << "lexema"
            << std::setw(W_mean) << "mean"
            << std::setw(W_LINE) << "line"
            << std::setw(W_IT) << "idxIT"
            << std::endl;

        std::string sep((size_t)W_IDX + W_LEX + W_mean + W_LINE + W_IT, '-');
        *log.stream << sep << std::endl;

        for (int i = 0; i < lt.size; ++i)
        {
            LT::Entry& e = lt.table[i];
            std::string lex = LexemeToString(e.lexema);
            const char* mean = MeanToString((unsigned char)e.sign);

            *log.stream << std::left
                << std::setw(W_IDX) << i
                << std::setw(W_LEX) << lex
                << std::setw(W_mean) << mean
                << std::setw(W_LINE) << e.sn
                << std::setw(W_IT) << ((e.idxIT == IT_NULLIDX) ? "-" : std::to_string(e.idxIT));

            // если есть связь с IT и передали таблицу идентификаторов — вывести доп.информацию
            if (it != nullptr && e.idxIT != IT_NULLIDX && e.idxIT >= 0 && e.idxIT < it->size)
            {
                IT::Entry& ident = it->table[e.idxIT];

                // значение как строка
                std::string valueStr = "???";
                if (ident.iddatatype == IT::INT)
                {
                    valueStr = "|" + std::to_string(ident.value.vint) + "|";
                }
                else if (ident.iddatatype == IT::STR)
                {
                    valueStr = "\"" + std::string(ident.value.vstr) + "\"";
                }
                else if (ident.iddatatype == IT::CHR)
                {
                    char tmp[8] = { 0,0,0,0,0,0,0,0 };
                    tmp[0] = ident.value.vchr;
                    valueStr = std::string("'") + tmp[0] + "'";
                }

                *log.stream << " -> IT: id=" << std::setw(W_NAME) << ident.id
                    << " scope=" << std::setw(W_SCOPE) << ident.scope
                    << " type=" << std::setw(W_TYPE) << IdTypeToString(ident.idtype)
                    << " dtype=" << std::setw(W_DATATYPE) << IdDataTypeToString(ident.iddatatype)
                    << " value=" << std::setw(W_VALUE) << valueStr << std::endl;
            }
            else
            {
                *log.stream << std::endl;
            }
        }

        *log.stream << std::endl << std::endl;
    }

    void WriteLex(LOG log, LT::LexTable& lt, IT::IdTable* it)
    {
        if (!log.stream) return;

        char ltsize[32];
        sprintf_s(ltsize, "%d", lt.size);

        WriteLine(log, (char*)"\n---------- Только лексемы -----------", (char*)"", "");
        WriteLine(log, (char*)"Всего лексем: ", (char*)ltsize, (char*)"", "");

        int currLine = -1;

        for (int i = 0; i < lt.size; ++i)
        {
            LT::Entry& e = lt.table[i];
            char lex = e.lexema;

            if (currLine != e.sn)
            {
                *log.stream << std::endl << std::setw(2) << e.sn << ": ";
                currLine = e.sn;
            }

            *log.stream << lex;


        }

        *log.stream << std::endl << std::endl;
    }

    void WriteIdTable(LOG log, IT::IdTable& it)
    {
        if (!log.stream) return;

        WriteLine(log, (char*)"------ Таблица идентификаторов ------", (char*)"", "");

        char itsize[32];
        sprintf_s(itsize, "%d", it.size);
        WriteLine(log, (char*)"Всего идентификаторов: ", (char*)itsize, (char*)"", "");

        const int W_INDEX = 8;
        const int W_NAME = 28;
        const int W_SCOPE = 20;
        const int W_TYPE = 16;
        const int W_DATATYPE = 12;
        const int W_VALUE = 16;

        *log.stream << std::left
            << std::setw(W_INDEX) << "idxLT"
            << std::setw(W_NAME) << "name"
            << std::setw(W_SCOPE) << "scope"
            << std::setw(W_TYPE) << "IdType"
            << std::setw(W_DATATYPE) << "DataType"
            << std::setw(W_VALUE) << "value"
            << std::endl;

        std::string separator = std::string(W_INDEX + W_NAME + W_SCOPE + W_TYPE + W_DATATYPE + W_VALUE, '-');
        *log.stream << separator << std::endl;

        for (int i = 0; i < it.size; ++i)
        {
            IT::Entry& entry = it.table[i];

            std::string valueStr = "???";
            if (entry.iddatatype == IT::INT)
            {
                valueStr = std::to_string(entry.value.vint);
            }
            else if (entry.iddatatype == IT::STR)
            {
                valueStr = std::string(entry.value.vstr);
            }
            else if (entry.iddatatype == IT::CHR)
            {
                char tmp[2] = { entry.value.vchr, 0 };
                valueStr = std::string("'") + tmp[0] + "'";
            }

            *log.stream << std::left
                << std::setw(W_INDEX) << entry.idxfirstLT
                << std::setw(W_NAME) << entry.id
                << std::setw(W_SCOPE) << entry.scope
                << std::setw(W_TYPE) << IdTypeToString(entry.idtype)
                << std::setw(W_DATATYPE) << IdDataTypeToString(entry.iddatatype)
                << std::setw(W_VALUE) << valueStr
                << std::endl;
        }

        *log.stream << std::endl << std::endl;
    }

    void Close(LOG log)
    {
        if (log.stream)
        {
        }
    }
} // namespace Log
