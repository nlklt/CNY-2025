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

namespace Log {// --------------------- Вспомогательные функции (исправлённые) ---------------------

    static std::string TruncatePad(const std::string& s, size_t width) {
        if (width == 0) return "";
        if (s.size() <= width) {
            std::string out = s;
            out.append(width - s.size(), ' ');
            return out;
        }
        if (width <= 3) return std::string(width, '.');
        // оставляем место для "..."
        std::string out = s.substr(0, width - 3);
        out += "...";
        return out;
    }

    static std::string SafeVStr(const char* s) {
        if (!s) return "";
        // больше не используем фиксированный большой лимит; обрезим при выводе через TruncatePad
        return std::string(s);
    }

    static std::string LexemeToString(char lex) {
        switch (lex) {
        case LT_MAIN:        return "main";
        case LT_TYPE:        return "type";
        case LT_FOR:         return "for";
        case LT_FUNCTION:    return "function";
        case LT_RETURN:      return "return";
        case LT_ID:          return "id";
        case LT_LITERAL:     return "lit";
        case LT_EQUAL:       return "=";
        case LT_OP_UNARY:    return "op_un";
        case LT_OP_BINARY:   return "op_bin";
        case LT_RANGE:       return "..";
        case LT_COMMA:       return ",";
        case LT_SEMICOLON:   return ";";
        case LT_LEFTBRACE:   return "{";
        case LT_RIGHTBRACE:  return "}";
        case LT_LEFTHESIS:   return "(";
        case LT_RIGHTHESIS:  return ")";
        default: {
            std::ostringstream ss;
            if (std::isprint(static_cast<unsigned char>(lex))) ss << lex;
            else ss << "0x" << std::hex << (int)(unsigned char)lex;
            return ss.str();
        }
        }
    }

    static std::string SignToString(unsigned char sign) {
        using SIGN = LT::SIGNATURE;
        switch (sign) {
        case SIGN::sign:               return "sign";
        case SIGN::t_int:              return "t_int";
        case SIGN::t_char:             return "t_char";
        case SIGN::t_string:           return "t_string";
        case SIGN::variable:           return "variable";
        case SIGN::function:           return "function";
        case SIGN::call:               return "call";
        case SIGN::parameter:          return "parameter";
        case SIGN::print:              return "print";
        case SIGN::date:               return "date";
        case SIGN::time:               return "time";
        case SIGN::increment:          return "inc";
        case SIGN::dicrement:          return "dec";
        case SIGN::inversion:          return "inv";
        case SIGN::increment_post:     return "inc_post";
        case SIGN::dicrement_post:     return "dec_post";
        case SIGN::inversion_post:     return "inv_post";
        case SIGN::pref_increment:     return "pref_inc";
        case SIGN::pref_dicrement:     return "pref_dec";
        case SIGN::pref_inversion:     return "pref_inv";
        case SIGN::plus:               return "+";
        case SIGN::minus:              return "-";
        case SIGN::division:           return "/";
        case SIGN::multiplication:     return "*";
        default:
            return "unknown_sign";
        }
    }

    static std::string IdTypeToString(IT::IDTYPE t) {
        switch (t) {
        case IT::V: return "VAR";
        case IT::F: return "FUNC";
        case IT::P: return "PARAM";
        case IT::C: return "CONST";
        case IT::L: return "LIT";
        default:    return "UNKNOWN";
        }
    }

    static std::string IdDataTypeToString(IT::IDDATATYPE t) {
        switch (t) {
        case IT::INT:  return "INT";
        case IT::CHR:  return "CHR";
        case IT::STR:  return "STR";
        case IT::ANY:  return "ANY";
        case IT::NONE: return "NONE";
        case IT::UNDEF:return "UNDEF";
        default: return "UNK";
        }
    }

    // WriteLexTable

    void WriteLexTable(std::ostream& out, const LT::LexTable& lt, const IT::IdTable* it) {
        if (!out) return;

        out << "---------- Таблица лексем (LT) ----------\n";
        out << "Maxsize: " << lt.maxsize << "    Size: " << lt.size << "\n\n";

        const int W_IDX = 6;
        const int W_LEX = 14; 
        const int W_SIGN = 14; 
        const int W_SN = 6;
        const int W_TN = 6;
        const int W_IT = 8;

        out << std::left
            << TruncatePad("Idx", W_IDX)
            << TruncatePad("lexema", W_LEX)
            << TruncatePad("sign", W_SIGN)
            << TruncatePad("sn", W_SN)
            << TruncatePad("tn", W_TN)
            << TruncatePad("idxIT", W_IT)
            << "\n";

        out << std::string(W_IDX + W_LEX + W_SIGN + W_SN + W_TN + W_IT, '-') << "\n";

        for (int i = 0; i < lt.size; ++i) {
            const LT::Entry& e = lt.table[i];

            std::string lex = LexemeToString(e.lexema);
            std::string sign = SignToString(static_cast<unsigned char>(e.sign));
            std::string idxITstr = (e.idxIT == IT_NULLIDX || e.idxIT < 0) ? "-" : std::to_string(e.idxIT);

            out << std::left
                << TruncatePad(std::to_string(i), W_IDX)
                << TruncatePad(lex, W_LEX)
                << TruncatePad(sign, W_SIGN)
                << TruncatePad(std::to_string(e.sn), W_SN)
                << TruncatePad(std::to_string(e.tn), W_TN)
                << TruncatePad(idxITstr, W_IT);

            if (it != nullptr && e.idxIT != IT_NULLIDX && e.idxIT >= 0 && e.idxIT < it->size) {
                const IT::Entry& ident = it->table[e.idxIT];

                std::string valueStr = "-";
                if (ident.iddatatype == IT::INT) valueStr = std::to_string(ident.value.vint);
                else if (ident.iddatatype == IT::STR) valueStr = "\"" + SafeVStr(ident.value.vstr) + "\"";
                else if (ident.iddatatype == IT::CHR) {
                    std::ostringstream tmp; tmp << "'" << ident.value.vchr << "'"; valueStr = tmp.str();
                }
            }

            out << "\n";
        }

        out << "\n";
    }

    // WriteIdTable

    void WriteIdTable(std::ostream& out, const IT::IdTable& it) {
        if (!out) return;

        out << "------- Таблица идентификаторов (IT) -------\n";
        out << "Maxsize: " << it.maxsize << "    Size: " << it.size << "\n\n";

        const int W_IDX = 6;
        const int W_ID = 90;
        const int W_FULL = 28;
        const int W_ITYPE = 8;
        const int W_DTYPE = 8;
        const int W_DEFINED = 9;

        out << std::left
            << TruncatePad("Idx", W_IDX)
            << TruncatePad("id", W_ID)
            << TruncatePad("fullName", W_FULL)
            << TruncatePad("idType", W_ITYPE)
            << TruncatePad("dataType", W_DTYPE)
            << TruncatePad("defined", W_DEFINED)
            << " value / params"
            << "\n";

        out << std::string(W_IDX + W_ID + W_FULL + W_ITYPE + W_DTYPE + W_DEFINED + 20, '-') << "\n";

        for (int i = 0; i < it.size; ++i) {
            const IT::Entry& e = it.table[i];

            std::string valueStr = "-";
            if (e.iddatatype == IT::INT) valueStr = std::to_string(e.value.vint);
            else if (e.iddatatype == IT::STR) valueStr = "\"" + SafeVStr(e.value.vstr) + "\"";
            else if (e.iddatatype == IT::CHR) {
                std::ostringstream tmp; tmp << "'" << e.value.vchr << "'"; valueStr = tmp.str();
            }

            out << std::left
                << TruncatePad(std::to_string(i), W_IDX)
                << TruncatePad(e.id, W_ID)
                << TruncatePad(e.fullName, W_FULL)
                << TruncatePad(IdTypeToString(e.idtype), W_ITYPE)
                << TruncatePad(IdDataTypeToString(e.iddatatype), W_DTYPE)
                << TruncatePad((e.isDefined ? "yes" : "no"), W_DEFINED);

            out << " idxFirstLT=" << (e.idxfirstLT >= 0 ? std::to_string(e.idxfirstLT) : std::string("-"));
            out << " value=" << TruncatePad(valueStr, 18);

            out << " paramsCount=" << e.params.count;
            if (e.params.count > 0) {
                out << " (";
                for (size_t pi = 0; pi < e.params.types.size(); ++pi) {
                    out << IdDataTypeToString(e.params.types[pi]);
                    if (pi + 1 < e.params.types.size()) out << ",";
                }
                out << ")";
            }

            out << "\n";
        }

        out << "\n";
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
                WriteLine(log, (char*)"Ошибка: ", (char*)error.message, (char*)"  строка: ", (char*)linebuf, (char*)"  позиция/токен: ", (char*)colbuf, (char*)"", "");
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

        WriteLine(log, (char*)"\n\n---------- Таблица лексем после полиза -----------", (char*)"", "");

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
}
