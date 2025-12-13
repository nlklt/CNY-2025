#include "error.h"
#include "lexer.h"
#include "in.h"
#include "lt.h"
#include "it.h"

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <cstring>

namespace Lexer
{
    void lexicalAnalysis(In::IN& in, LT::LexTable& lt, IT::IdTable& it)
    {
        std::map<std::string, char> keywords;

        keywords["int"]       = LT_TYPE;
        keywords["char"]      = LT_TYPE;
        keywords["string"]    = LT_TYPE;

        keywords["main"]      = LT_MAIN;

        keywords["for"]       = LT_FOR;

        keywords["function"]  = LT_FUNCTION;
        keywords["return"]    = LT_RETURN;

        keywords["print"]     = LT_ID;
        keywords["get_time"]  = LT_ID;
        keywords["get_date"]  = LT_ID;

        // стек областей видимости
        std::vector<std::string> scopeStack;
        scopeStack.push_back("global"); // глобальная область
        int scopeScp = 0;

        int tn        = 0;
        int line      = 1;
        int position  = 0;

        // контекстные флаги
        bool afterFunctionKeyword   = false;
        bool afterFunctionName      = false;
        bool inFunctionHeader       = false;

        IT::IDDATATYPE lastTypeToken = IT::IDDATATYPE::UNDEF;

        for (int i = 0; i < in.size; ++i) {
            char c = in.text[i];

            if (c == '|') {
                ++line; tn = 0; position = 0;
                continue;
            }

            else if (isspace(c)) {
                ++position;
                continue;
            }

            else if (isalpha(c) || c == '_') { // ключевое слово или идентификатор
                std::string word;
                while (i < in.size && (isalnum(in.text[i]) || in.text[i] == '_')) {
                    word += in.text[i];
                    ++i; ++position;
                }
                --i; --position; //вернём на последний символ слова

                if (keywords.count(word)) { // ключевое слово
                    char kw     = keywords[word];
                    char sign   = kw;

                    if      (word == "int")     { lastTypeToken = IT::IDDATATYPE::INT;   sign = LT::SIGNATURE::t_int;    }
                    else if (word == "char")    { lastTypeToken = IT::IDDATATYPE::CHR;   sign = LT::SIGNATURE::t_char;   }
                    else if (word == "string")  { lastTypeToken = IT::IDDATATYPE::STR;   sign = LT::SIGNATURE::t_string; }
                    else if (word == "print")   { sign = LT::SIGNATURE::print; }
                    else if (word == "get_time"){ sign = LT::SIGNATURE::time;  }
                    else if (word == "get_date"){ sign = LT::SIGNATURE::date;  }
                    else                        { lastTypeToken = IT::IDDATATYPE::UNDEF; }

                    if      (word == "main")     { scopeStack.push_back("main"); }
                    else if (word == "for")      { scopeStack.push_back(std::to_string(scopeScp++)); }
                    
                    if (kw == LT_FUNCTION) afterFunctionKeyword = true;

                    if (word == "print" || word == "get_date" || word == "get_time") {
                        IT::Entry identry_i(lt.size, word, word, IT::IDTYPE::C, IT::IDDATATYPE::NONE);
                        identry_i.isDefined = true;

                        if (word == "print") {
                            identry_i.params.count = 1;
                            identry_i.params.types = { IT::IDDATATYPE::ANY };
                        }
                        if (word == "get_time" || word == "get_date") {
                            identry_i.params.count = 0;
                            identry_i.params.types = {};
                        }
                        IT::Add(it, identry_i);

                        LT::Entry kwentry_l(kw, sign, line, ++tn, it.size - 1);
                        LT::Add(lt, kwentry_l);
                    }
                    
                    else {
                        LT::Entry kwentry_l(kw, sign, line, ++tn, LT_NULLIDX);
                        LT::Add(lt, kwentry_l);
                    }
                    continue;
                }
                else {  // идентификатор
                    char lexema = LT_ID;

                    // область видимости
                    std::string currScope = scopeStack.back();
                    std::string fullName;
                    fullName = currScope + "$" + word;

                    if (fullName.length() >= IT_ID_MAXSIZE * 2) {
                        fullName = fullName.substr(0, (IT_ID_MAXSIZE * 2 - 1));
                    }

                    // наличие в таблице с учетом области видимости
                    std::vector<std::string> copyScope = scopeStack;
                    int idxIT = IT_NULLIDX;
                    while (!copyScope.empty()) {
                        currScope = copyScope.back();
                        copyScope.pop_back();
                        fullName = currScope + "$" + word;

                        idxIT = IT::IsId(it, fullName);
                        if (idxIT != IT_NULLIDX) break;
                    }

                    if (idxIT == IT_NULLIDX) {
                        IT::IDTYPE     decidedType = IT::IDTYPE::V;
                        IT::IDDATATYPE decidedDataType = lastTypeToken;

                        if (afterFunctionKeyword) {     // функция
                            decidedType = IT::IDTYPE::F;
                            afterFunctionKeyword = false;
                            afterFunctionName = true;
                            scopeStack.push_back(word);
                        }
                        else if (inFunctionHeader) {    // параметр
                            decidedType = IT::IDTYPE::P;
                        }
                        fullName = fullName.substr(0, (IT_ID_MAXSIZE * 2 - 1));

                        IT::Entry identry_i(lt.size, word, fullName, decidedType, decidedDataType);
                        if      (decidedDataType == IT::IDDATATYPE::INT)    identry_i.value.vint = IT_INT_DEFAULT;
                        else if (decidedDataType == IT::IDDATATYPE::CHR)    identry_i.value.vchr = IT_CHAR_DEFAULT;
                        else if (decidedDataType == IT::IDDATATYPE::STR)    strcpy_s(identry_i.value.vstr, sizeof(identry_i.value.vstr), IT_STR_DEFAULT);

                        IT::Add(it, identry_i);
                        idxIT = it.size - 1;
                    }
                    else if (it.table[idxIT].idtype == IT::IDTYPE::F) {
                        IT::Entry callentry_i = it.table[idxIT];
                        callentry_i.idtype = IT::IDTYPE::C;
                    }
                    else if (lt.table[lt.size - 1].lexema == LT_TYPE) {
                        std::cerr << "Повторное создание переменной\n";
                    }

                    LT::Entry identry_l(lexema, LT_ID, line, ++tn, idxIT);
                    LT::Add(lt, identry_l);
                    continue;
                }
            }

            else if (isdigit(c)) { // числовой литерал
                if (in.text[i] == '0' && i + 1 < in.size && in.text[i + 1] == 'o') { // восьмеричный
                    i += 2; position += 2;

                    std::string oct;
                    while (i < in.size && in.text[i] >= '0' && in.text[i] <= '7') {
                        oct += in.text[i];
                        ++i; ++position;
                    }
                    --i; --position;

                    if (oct.empty()) ERROR_THROW(201);

                    long long value = 0;
                    try {
                        for (char ch : oct) {
                            value = value * 8 + (ch - '0');
                        }
                    }
                    catch (...) { ERROR_THROW(201); }

                    if (lt.table[lt.size - 1].sign == LT::SIGNATURE::minus) {
                        value *= (-1);
                        oct += "_n";
                    }
                    std::string number = std::string("0o") + oct;
                    
                    std::string fullName = "L_num_" + number;

                    int idxIT = IT::IsId(it, fullName);
                    if (idxIT == IT_NULLIDX) { 
                        IT::Entry octentry_i(lt.size, number, fullName, IT::IDTYPE::L, IT::IDDATATYPE::INT);
                        octentry_i.value.vint = (int)value;
                        IT::Add(it, octentry_i);
                        idxIT = it.size - 1;
                    }

                    char sign = LT::SIGNATURE::number;
                    LT::Entry octentry_l(LT_LITERAL, sign, line, ++tn, idxIT);
                    if (lt.table[lt.size - 1].sign == LT::SIGNATURE::minus) {
                        lt.table[lt.size - 1] = octentry_l;
                    }
                    else {
                        LT::Add(lt, octentry_l);
                    }
                    continue;
                }
                else { // десятичный литерал
                    std::string dec;
                    while (i < in.size && isdigit(in.text[i])) {
                        dec += in.text[i];
                        ++i; ++position;
                    }
                    --i; --position;

                    long long value = 0;
                    try { value = std::stoll(dec); }
                    catch (...) { ERROR_THROW(201); }

                    if (lt.table[lt.size - 1].sign == LT::SIGNATURE::minus) {
                        value *= (-1);
                        dec += "_n";
                    }
                    std::string fullName = "L_num_" + dec;
                    int idxIT = IT::IsId(it, fullName);
                    if (idxIT == IT_NULLIDX) {
                        IT::Entry decentry_i(lt.size, dec, fullName, IT::IDTYPE::L, IT::IDDATATYPE::INT);
                        decentry_i.value.vint = (int)value;
                        IT::Add(it, decentry_i);
                        idxIT = it.size - 1;
                    }

                    char sign = LT::SIGNATURE::number;
                    LT::Entry decentry_l(LT_LITERAL, sign, line, ++tn, idxIT);
                    if (lt.table[lt.size - 1].sign == LT::SIGNATURE::minus) {
                        lt.table[lt.size - 1] = decentry_l;
                    }
                    else {
                        LT::Add(lt, decentry_l);
                    }
                    continue;
                }
            }

            else if (c == '\"') { // строковый литерал
                i++; position++;

                std::string str;
                while (i < in.size && in.text[i] != '\"') {
                    str += in.text[i];
                    i++; position++;
                }

                if (i >= in.size || in.text[i] != '\"') 
                    ERROR_THROW(203, line, position);

                if (str.length() > IT_STR_MAXSIZE)      
                    ERROR_THROW(203, line, position);

                std::string fullName = "L_str_" + std::to_string(lt.size);
                int idxIT = IT::IsId(it, fullName);
                if (idxIT == IT_NULLIDX) {
                    IT::Entry strentry_i(lt.size, str, fullName, IT::IDTYPE::L, IT::IDDATATYPE::STR);
                    strcpy_s(strentry_i.value.vstr, sizeof(strentry_i.value.vstr), str.c_str());

                    IT::Add(it, strentry_i);
                    idxIT = it.size - 1;
                }

                char sign = LT::SIGNATURE::string;
                LT::Entry strentry_l(LT_LITERAL, sign, line, ++tn, idxIT);
                LT::Add(lt, strentry_l);
            }

            else if (c == '\'') { // символьный литерал
                i++; position++;

                std::string chr;
                while (i < in.size && in.text[i] != '\'') {
                    chr += in.text[i];
                    i++; position++;
                }

                if (i >= in.size || in.text[i] != '\'' || chr.length() != 1) 
                    ERROR_THROW(203, line, colStart);

                if (chr.length() != 1) 
                    ERROR_THROW(203, line, position);

                std::string fullName = "L_chr_" + std::to_string(lt.size);
                int idxIT = IT::IsId(it, fullName);
                if (idxIT == IT_NULLIDX) {
                    IT::Entry strentry_i(lt.size, chr, fullName, IT::IDTYPE::L, IT::IDDATATYPE::CHR);
                    strentry_i.value.vchr = chr[0];

                    IT::Add(it, strentry_i);
                    idxIT = it.size - 1;
                }

                char sign = LT::SIGNATURE::symbol;
                LT::Entry strentry_l(LT_LITERAL, sign, line, ++tn, idxIT);
                LT::Add(lt, strentry_l);
                continue;
            }
            else {// символ/..
                char symbol = in.text[i];
                char sign = symbol;

                switch (symbol) {
                    case '{': symbol = LT_LEFTBRACE; break;
                    case '}': symbol = LT_RIGHTBRACE;
                        if (scopeStack.size() > 1)
                        {
                            scopeStack.pop_back();
                        }
                        break;
                    case '(': symbol = LT_LEFTHESIS;
                        if (afterFunctionName)
                        {
                            inFunctionHeader = true;
                            afterFunctionName = false;
                        }
                        break;
                    case ')': symbol = LT_RIGHTHESIS;
                        if (inFunctionHeader)
                        {
                            inFunctionHeader = false;
                            lastTypeToken = IT::IDDATATYPE::UNDEF;
                        }
                        break;
                    case '.':
                        if (i + 1 < in.size && in.text[i + 1] == '.')
                        {
                            symbol = LT_RANGE;
                            ++i; ++position;
                        }
                        else ERROR_THROW(205, line, position);
                        break;
                    case '+':
                        if (i + 1 < in.size && in.text[i + 1] == '+')
                        {
                            symbol = LT_OP_UNARY;
                            sign = LT::SIGNATURE::increment;
                            ++i; ++position;
                        }
                        else {
                            symbol = LT_OP_BINARY;
                            sign = LT::SIGNATURE::plus;
                        }
                        break;
                    case '-':
                        if (i + 1 < in.size && in.text[i + 1] == '-')
                        {
                            symbol = LT_OP_UNARY;
                            sign = LT::SIGNATURE::dicrement;
                            ++i; ++position;
                        }
                        else {
                            symbol = LT_OP_BINARY;
                            sign = LT::SIGNATURE::minus;
                        }
                        break;
                    case '*':
                        symbol = LT_OP_BINARY;
                        sign = LT::SIGNATURE::multiplication;
                        break;
                    case '/':
                        symbol  = LT_OP_BINARY;
                        sign    = LT::SIGNATURE::division;
                        break;
                    case ',': symbol = LT_COMMA; break;
                    case ';':
                        symbol = LT_SEMICOLON;
                        lastTypeToken = IT::IDDATATYPE::UNDEF;
                        break;
                    case '~': symbol = LT_OP_UNARY; sign = LT::SIGNATURE::inversion; break;
                    case '=': symbol = LT_EQUAL; break;
                    default:
                        ERROR_THROW(205, line, position);
                }

                LT::Entry symbentry_l(symbol, sign, line, ++tn, IT_NULLIDX);
                symbentry_l.lexema = symbol;
                
                LT::Add(lt, symbentry_l);
            }
        }
    }
}