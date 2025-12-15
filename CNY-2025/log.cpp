#include "log.h"

#include <iostream>
#include <fstream>
#include <cstdarg>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <clocale>
#include <cstring>
#include <map>

namespace Log {
    // --- Вспомогательные функции преобразования в строку ---
    const char* LexemeToString(char lex) {
        static char buf[8];
        switch (lex) {
        case LT_EOF:       return "EOF";
        case LT_MAIN:      return "main";
        case LT_TYPE:      return "type";
        case LT_FOR:       return "for";
        case LT_FUNCTION:  return "function";
        case LT_RETURN:    return "return";
        case LT_ID:        return "id";
        case LT_LITERAL:   return "literal";
        case LT_EQUAL:     return "=";
        case LT_OP_UNARY:  return "unary_op";
        case LT_OP_BINARY: return "binary_op";
        case LT_RANGE:     return ".";
        case LT_COMMA:     return ",";
        case LT_SEMICOLON: return ";";
        case LT_LEFTBRACE: return "{";
        case LT_RIGHTBRACE:return "}";
        case LT_LEFTHESIS: return "(";
        case LT_RIGHTHESIS:return ")";
        default:
            // если символ печатный — вернуть как односимвольную строку
            if (isprint(static_cast<unsigned char>(lex))) {
                buf[0] = lex;
                buf[1] = '\0';
                return buf;
            }
            return "unknown";
        }
    }

    const char* MeanToString(unsigned char sign) {
        // см. LT::SIGNATURE — перечислить основные варианты
        switch (sign) {
        case LT::SIGNATURE::sign:        return "sign";
        case LT::SIGNATURE::t_int:       return "t_int";
        case LT::SIGNATURE::t_char:      return "t_char";
        case LT::SIGNATURE::t_string:    return "t_string";
        case LT_LITERAL:                 return "literal";
        case LT::SIGNATURE::print:       return "print";
        case LT::SIGNATURE::date:        return "date";
        case LT::SIGNATURE::time:        return "time";
        case LT::SIGNATURE::increment:   return "increment";
        case LT::SIGNATURE::dicrement:   return "dicrement";
        case LT::SIGNATURE::inversion:   return "inversion";
        case LT::SIGNATURE::plus:        return "plus";
        case LT::SIGNATURE::minus:       return "minus";
        case LT::SIGNATURE::division:    return "division";
        case LT::SIGNATURE::multiplication: return "multiplication";
        default: return "unknown_sign";
        }
    }

    const char* IdTypeToString(IT::IDTYPE t) {
        switch (t) {
        case IT::V: return "VAR";
        case IT::F: return "FUNC";
        case IT::P: return "PARAM";
        case IT::C: return "CONST";
        case IT::L: return "LABEL";
        default:    return "UNKNOWN";
        }
    }

    const char* IdDataTypeToString(IT::IDDATATYPE dt) {
        switch (dt) {
        case IT::INT:  return "INT";
        case IT::CHR:  return "CHR";
        case IT::STR:  return "STR";
        case IT::NONE: return "NONE";
        case IT::UNDEF: return "UNDEF";
        default: return "UNK_DTYPE";
        }
    }

    // --- Полезная утилита для безопасного получения C-string из value.vstr ---
    std::string SafeVStr(const char* vstr) {
        if (!vstr) return "";
        size_t n = strnlen(vstr, IT_STR_MAXSIZE);
        return std::string(vstr, n);
    }

    // --- Функция вывода таблицы лексем (расширенная) ---
    void WriteLexTable(LOG log, LT::LexTable& lt, IT::IdTable* it)
    {
        if (!log.stream) return;
        std::ostream& out = *log.stream;

        out << "---------- Таблица лексем (LT) ----------" << std::endl;
        out << "Всего лексем: " << lt.size << std::endl << std::endl;

        // Ширины колонок
        const int W_IDX = 6;
        const int W_LEX = 12;
        const int W_MEAN = 18;
        const int W_SN = 6;
        const int W_TN = 6;
        const int W_IT = 6;

        out << std::left
            << std::setw(W_IDX) << "Idx"
            << std::setw(W_LEX) << "lexema"
            << std::setw(W_MEAN) << "mean"
            << std::setw(W_SN) << "sn"
            << std::setw(W_TN) << "tn"
            << std::setw(W_IT) << "idxIT"
            << std::endl;

        std::string sep(W_IDX + W_LEX + W_MEAN + W_SN + W_TN + W_IT, '-');
        out << sep << std::endl;

        for (int i = 0; i < lt.size; ++i) {
            LT::Entry& e = lt.table[i];
            std::string lex = LexemeToString(e.lexema);
            const char* mean = MeanToString(static_cast<unsigned char>(e.sign));

            out << std::left
                << std::setw(W_IDX) << i
                << std::setw(W_LEX) << lex
                << std::setw(W_MEAN) << mean
                << std::setw(W_SN) << e.sn
                << std::setw(W_TN) << e.tn
                << std::setw(W_IT) << ((e.idxIT == IT_NULLIDX) ? "-" : std::to_string(e.idxIT));

            // Если есть связь с IT и передана таблица идентификаторов — вывести расширенную информацию
            if (it != nullptr && e.idxIT != IT_NULLIDX && e.idxIT >= 0 && e.idxIT < it->size) {
                IT::Entry& ident = it->table[e.idxIT];

                // значение как строка
                std::string valueStr = "N/A";
                if (ident.iddatatype == IT::INT) {
                    valueStr = "|" + std::to_string(ident.value.vint) + "|";
                }
                else if (ident.iddatatype == IT::STR) {
                    valueStr = "\"" + SafeVStr(ident.value.vstr) + "\"";
                }
                else if (ident.iddatatype == IT::CHR) {
                    char tmp = ident.value.vchr;
                    std::ostringstream oss;
                    oss << "'" << tmp << "'";
                    valueStr = oss.str();
                }
                else {
                    valueStr = "-";
                }

                out << " -> IT: id=\"" << std::setw(20) << ident.id
                    << " type=" << std::setw(6) << IdTypeToString(ident.idtype)
                    << " dtype=" << std::setw(6) << IdDataTypeToString(ident.iddatatype)
                    << " value=" << std::setw(12) << valueStr;

                // idxfirstLT
                out << " idxFirstLT=" << (ident.idxfirstLT >= 0 ? std::to_string(ident.idxfirstLT) : std::string("-"));

                // params (если есть)
                out << " paramsCount=" << ident.params.count;
                if (ident.params.count > 0) {
                    out << " (";
                    for (size_t pi = 0; pi < ident.params.types.size(); ++pi) {
                        out << IdDataTypeToString(ident.params.types[pi]);
                        if (pi + 1 < ident.params.types.size()) out << ",";
                    }
                    out << ")";
                }

                // fullName и isDefined
                out << " fullName=\"" << ident.fullName << "\"";
                out << " defined=" << (ident.isDefined ? "yes" : "no");

                out << std::endl;
            }
            else {
                out << std::endl;
            }
        }

        out << std::endl;
    }

    // --- Функция вывода таблицы идентификаторов (IT) ---
    void WriteIdTable(LOG log, IT::IdTable& it, LT::LexTable* lt)
    {
        if (!log.stream) return;
        std::ostream& out = *log.stream;

        out << "---------- Таблица идентификаторов (IT) ----------" << std::endl;
        out << "Всего идентификаторов: " << it.size << std::endl << std::endl;

        // Заголовки
        const int W_IDX = 6;
        const int W_NAME = 22;
        const int W_SCOPE = 12;
        const int W_IDTYPE = 8;
        const int W_IDDT = 8;
        const int W_IDXFIRST = 10;

        out << std::left
            << std::setw(W_IDX) << "Idx"
            << std::setw(W_NAME) << "id"
            << std::setw(W_IDTYPE) << "idType"
            << std::setw(W_IDDT) << "dtype"
            << std::setw(W_IDXFIRST) << "idxFirstLT"
            << std::endl;

        std::string sep(W_IDX + W_NAME + W_SCOPE + W_IDTYPE + W_IDDT + W_IDXFIRST, '-');
        out << sep << std::endl;

        for (int i = 0; i < it.size; ++i) {
            IT::Entry& e = it.table[i];

            out << std::left
                << std::setw(W_IDX) << i
                << std::setw(W_NAME) << e.id
                << std::setw(W_IDTYPE) << IdTypeToString(e.idtype)
                << std::setw(W_IDDT) << IdDataTypeToString(e.iddatatype)
                << std::setw(W_IDXFIRST) << (e.idxfirstLT >= 0 ? std::to_string(e.idxfirstLT) : std::string("-"));

            // value
            std::string valueStr = "N/A";
            if (e.iddatatype == IT::INT) {
                valueStr = std::to_string(e.value.vint);
            }
            else if (e.iddatatype == IT::STR) {
                valueStr = "\"" + SafeVStr(e.value.vstr) + "\"";
            }
            else if (e.iddatatype == IT::CHR) {
                std::ostringstream oss;
                oss << "'" << e.value.vchr << "'";
                valueStr = oss.str();
            }
            out << " value=" << std::setw(14) << valueStr;

            // Параметры (для функций)
            out << " paramsCount=" << e.params.count;
            if (e.params.count > 0) {
                out << " (";
                for (size_t j = 0; j < e.params.types.size(); ++j) {
                    out << IdDataTypeToString(e.params.types[j]);
                    if (j + 1 < e.params.types.size()) out << ",";
                }
                out << ")";
            }

            // fullName и isDefined
            out << " fullName=\"" << e.fullName << "\"";
            out << " defined=" << (e.isDefined ? "yes" : "no");

            // опционально: показать внешние ссылки на LT (если передали lt)
            if (lt != nullptr && e.idxfirstLT >= 0 && e.idxfirstLT < lt->size) {
                out << " -> LT[" << e.idxfirstLT << "] lexema='" << LexemeToString(lt->table[e.idxfirstLT].lexema) << "'";
            }

            out << std::endl;
        }

        out << std::endl;
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

    void Close(LOG log)
    {
        if (log.stream)
        {
        }
    }
} // namespace Log
