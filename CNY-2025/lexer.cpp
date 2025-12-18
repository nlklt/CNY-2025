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

        keywords["int"] = LT_TYPE;
        keywords["string"] = LT_TYPE;

        keywords["main"] = LT_MAIN;

        keywords["for"] = LT_FOR;

        keywords["function"] = LT_FUNCTION;
        keywords["return"] = LT_RETURN;

        keywords["print"] = LT_ID;
        keywords["get_time"] = LT_ID;
        keywords["get_date"] = LT_ID;

        // стек областей видимости
        std::vector<std::string> scopeStack;
        scopeStack.push_back("global"); // глобальная область
        int scopeScp = 0;

        int tn = 0;         // номер токена
        int line = 1;       // строка
        int position = 0;   // позиция в строке

        // контекстные флаги
        bool afterFunctionKeyword = false;
        bool afterFunctionName = false;
        bool inFunctionHeader = false;
        bool isNegative = false;

        bool mainIsWas = false;

        IT::IDDATATYPE lastTypeToken = IT::IDDATATYPE::UNDEF;

        for (int i = 0; i < in.size; ++i) {
            char c = in.text[i];

            if (c == '|') {
                ++line; tn = 0; position = 0;
                continue;
            }

            else if (isspace(c)) {
                continue;
            }

            // ключевое слово или идентификатор
            else if (isalpha(c) || c == '_') {
                std::string word;
                while (i < in.size && (isalnum(in.text[i]) || in.text[i] == '_')) {
                    word += in.text[i];
                    ++i; ++position;
                }
                --i; --position; // возврат на последний символ слова

                if (word.length() > IT_ID_MAXSIZE) {
                    std::cout << Colors::YELLOW << "Warning: Идентифиукатор '" << word << 
                        "' слишком большой (максимальная длина " << 
                        IT_ID_MAXSIZE << "), идентификаор будет обрезан" << Colors::RESET << std::endl;
                    word = word.substr(0, (IT_ID_MAXSIZE - 1));
                }

                // ключевое слово
                if (keywords.count(word)) { 
                    char kw = keywords[word];
                    char sign = kw;

                    if      (word == "int")     { lastTypeToken = IT::IDDATATYPE::INT;   sign = LT::SIGNATURE::t_int; }
                    else if (word == "string")  { lastTypeToken = IT::IDDATATYPE::STR;   sign = LT::SIGNATURE::t_string; }
                    else if (word == "print")   { sign = LT::SIGNATURE::print; }
                    else if (word == "get_time"){ sign = LT::SIGNATURE::time; }
                    else if (word == "get_date"){ sign = LT::SIGNATURE::date; }
                    else    { lastTypeToken = IT::IDDATATYPE::UNDEF; }

                    if      (word == "main") { 
                        scopeStack.push_back("main"); 
                        if (mainIsWas) {
                            std::cout << Colors::RED << "Error: Обнаружено несколько точек входа main" << Colors::RESET << std::endl;
                            ERROR_THROW_IN(207, line, position);
                        }
                        mainIsWas = true;
                    }
                    else if (word == "for")  { scopeStack.push_back(std::to_string(scopeScp++)); }

                    if (kw == LT_FUNCTION) afterFunctionKeyword = true;

                    if (word == "print" || word == "get_date" || word == "get_time") {
                        IT::Entry identry_i(lt.size, word, word, IT::IDTYPE::F, IT::IDDATATYPE::STR);
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

                // идентификатор
                else { 
                    char lexema = LT_ID;

                    std::string currScope = scopeStack.back();
                    std::string fullName;
                    fullName = word + "$" + currScope;

                    if (fullName.length() >= IT_ID_MAXSIZE * 2) {
                        fullName = fullName.substr(0, (IT_ID_MAXSIZE * 2 - 1));
                    }

                    // наличие в таблице с учетом области видимости
                    std::vector<std::string> copyScope = scopeStack;
                    int idxIT = IT_NULLIDX;
                    while (!copyScope.empty() && lt.table[lt.size - 1].lexema != LT_TYPE) {
                        currScope = copyScope.back();
                        copyScope.pop_back();
                        fullName = word + "$" + currScope;

                        idxIT = IT::IsId(it, fullName);
                        if (idxIT != IT_NULLIDX) break;
                    }

                    bool previousIsType = (lt.size > 0 && lt.table[lt.size - 1].lexema == LT_TYPE);

                    if (previousIsType) {
                        std::string currentScopeName = word + "$" + scopeStack.back();
                        int existsInCurrentScope = IT::IsId(it, currentScopeName);

                        if (existsInCurrentScope != IT_NULLIDX) {
                            std::cout << Colors::RED << "Error:   Переобъявление переменной \"" << word << "\" в 1 облости \"" << scopeStack.back() << "\"" << Colors::RESET << std::endl;
                            ERROR_THROW_IN(200, line, position);
                        }
                    }

                    char sign = LT::SIGNATURE::variable;
                    if (afterFunctionKeyword) {
                        sign = LT::SIGNATURE::function;
                    }
                    else if (inFunctionHeader) {
                        sign = LT::SIGNATURE::parameter;
                    }

                    if (idxIT == IT_NULLIDX) {
                        IT::IDTYPE     decidedType = IT::IDTYPE::V;
                        IT::IDDATATYPE decidedDataType = lastTypeToken;

                        if (afterFunctionKeyword) {
                            decidedType = IT::IDTYPE::F;
                            afterFunctionKeyword = false;
                            afterFunctionName = true;
                            scopeStack.push_back(word);
                        }
                        else if (inFunctionHeader) {
                            decidedType = IT::IDTYPE::P;
                        }

                        IT::Entry identry_i(lt.size, word, fullName, decidedType, decidedDataType);
                        if      (decidedDataType == IT::IDDATATYPE::INT)    identry_i.value.vint = IT_INT_DEFAULT;
                        else if (decidedDataType == IT::IDDATATYPE::CHR)    identry_i.value.vchr = IT_CHAR_DEFAULT;
                        else if (decidedDataType == IT::IDDATATYPE::STR)    strcpy_s(identry_i.value.vstr, sizeof(identry_i.value.vstr), IT_STR_DEFAULT);

                        IT::Add(it, identry_i);
                        idxIT = it.size - 1;
                    }
                    else if (it.table[idxIT].idtype == IT::IDTYPE::F) {
                        sign = LT::SIGNATURE::call;
                        inFunctionHeader = true;
                    }

                    LT::Entry identry_l(lexema, sign, line, ++tn, idxIT);
                    LT::Add(lt, identry_l);
                    continue;
                }
            }

            // числовой литерал
            else if (isdigit(c) || (c == '-' && i + 1 < in.size && isdigit(in.text[i + 1]))) {
                if (c == '-') {
                    if (c == '-' && i + 1 < in.size && isdigit(in.text[i + 1])) {
                        isNegative = true;
                        i++; position++;
                    }
                }
                // восьмеричная запись
                if (in.text[i] == '0' && i + 1 < in.size && in.text[i + 1] == 'o') {
                    i += 2; position += 2;

                    std::string oct;
                    while (i < in.size && in.text[i] >= '0' && in.text[i] <= '7') {
                        oct += in.text[i];
                        ++i; ++position;
                    }
                    
                    if (isalpha(in.text[i]) || isdigit(in.text[i]) && in.text[i] >= 8) {
                        std::cout << Colors::RED << "Error: Недопустимый символ в записи восьмеричного числа '" <<
                            in.text[i] << "'" << Colors::RESET << std::endl;
                        ERROR_THROW_IN(206, line, position);
                    }
                    --i; --position;

                    long long value = 0;
                    try {
                        for (char ch : oct) {
                            value = value * 8 + (ch - '0');
                            if (value > IT_INT_MAXVALUE) {
                                std::cout << Colors::RED << "Error: Переполнение целочисленного тиипа в восьмеричном литерале " <<
                                    value << Colors::RESET << std::endl;
                                ERROR_THROW_IN(201, line, position);
                                break;
                            }
                        }
                    }
                    catch (...) { ERROR_THROW(201); }

                    if (isNegative) {
                        value *= (-1);
                        oct += "_n";
                        isNegative = false;
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

                    char sign = LT::SIGNATURE::variable;
                    if (inFunctionHeader) {
                        sign = LT::SIGNATURE::parameter;
                    }
                    LT::Entry octentry_l(LT_LITERAL, sign, line, ++tn, idxIT);
                    LT::Add(lt, octentry_l);
                    continue;
                }

                // десятичная запись
                else {
                    std::string dec;
                    while (i < in.size && isdigit(in.text[i])) {
                        dec += in.text[i];
                        ++i; ++position;
                    }
                    --i; --position;

                    long long value = 0;
                    try { value = std::stoll(dec); }
                    catch (...) { ERROR_THROW(201); }

                    if (value > IT_INT_MAXVALUE) {
                        std::cout << Colors::RED << "Error: Переполнение целочисленного тиипа в дестяичном литерале " <<
                            value << Colors::RESET << std::endl;
                        ERROR_THROW_IN(201, line, position);
                        break;
                    }

                    if (isNegative) {
                        value *= (-1);
                        dec += "_n";
                        isNegative = false;
                    }
                    std::string fullName = "L_num_" + dec;
                    int idxIT = IT::IsId(it, fullName);
                    if (idxIT == IT_NULLIDX) {
                        IT::Entry decentry_i(lt.size, dec, fullName, IT::IDTYPE::L, IT::IDDATATYPE::INT);
                        decentry_i.value.vint = (int)value;
                        IT::Add(it, decentry_i);
                        idxIT = it.size - 1;
                    }

                    char sign = LT::SIGNATURE::variable;
                    if (inFunctionHeader) {
                        sign = LT::SIGNATURE::parameter;
                    }
                    LT::Entry decentry_l(LT_LITERAL, sign, line, ++tn, idxIT);
                    LT::Add(lt, decentry_l);
                    continue;
                }
            }

            // строковый литерал
            else if (c == '\"') {
                i++; position++;

                std::string str;
                while (in.text[i] != '|' && i < in.size && in.text[i] != '\"') {
                    str += in.text[i];
                    i++; position++;
                }

                if (i >= in.size || in.text[i] != '\"') {
                    std::cout << Colors::RED << "Error: Незакрытый строковый литерал" << str << Colors::RESET << std::endl;
                     ERROR_THROW_IN(204, line, position);
                }

                if (str.length() > IT_STR_MAXSIZE) {
                    std::cout << Colors::RED << "Error: Слишком большой строковый литерал  (максимальное значение " <<
                        IT_STR_MAXSIZE << ")" << Colors::RESET << std::endl;
                    ERROR_THROW_IN(202, line, position);
                }

                std::string fullName = "L_str_" + std::to_string(lt.size);
                int idxIT = IT::IsId(it, fullName);
                if (idxIT == IT_NULLIDX) {
                    IT::Entry strentry_i(lt.size, str, fullName, IT::IDTYPE::L, IT::IDDATATYPE::STR);
                    strcpy_s(strentry_i.value.vstr, sizeof(strentry_i.value.vstr), str.c_str());

                    IT::Add(it, strentry_i);
                    idxIT = it.size - 1;
                }

                char sign = LT::SIGNATURE::variable;
                if (inFunctionHeader) {
                    sign = LT::SIGNATURE::parameter;
                }
                LT::Entry strentry_l(LT_LITERAL, sign, line, ++tn, idxIT);
                LT::Add(lt, strentry_l);
            }

            // символ/..
            else {
                char symbol = in.text[i];
                char sign = symbol;

                switch (symbol) {
                case '{': symbol = LT_LEFTBRACE; break;
                case '}': symbol = LT_RIGHTBRACE;
                    if (scopeStack.size() > 1) {
                        scopeStack.pop_back();
                    }
                    else {
                        std::cout << Colors::RED << "Error: Несоответствующая закрывающая скобка '}'" << 
                            Colors::RESET << std::endl;
                        ERROR_THROW_IN(203, line, position);
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
                    else ERROR_THROW_IN(205, line, position);
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
                    symbol = LT_OP_BINARY;
                    sign = LT::SIGNATURE::division;
                    break;
                case ',': symbol = LT_COMMA; break;
                case ';':
                    symbol = LT_SEMICOLON;
                    lastTypeToken = IT::IDDATATYPE::UNDEF;
                    break;
                case '~': symbol = LT_OP_UNARY; sign = LT::SIGNATURE::inversion; break;
                case '=': symbol = LT_EQUAL; break;
                default:
                    std::cout << Colors::RED << "Error: Неопределённый символ '" << symbol << "'" << Colors::RESET << std::endl;
                    ERROR_THROW_IN(205, line, position);
                }

                LT::Entry symbentry_l(symbol, sign, line, ++tn, IT_NULLIDX);
                symbentry_l.lexema = symbol;

                LT::Add(lt, symbentry_l);
            }
        }
        if (!mainIsWas) {
            std::cout << Colors::RED << "Error: Не обнаружено точки входа main" << Colors::RESET << std::endl;
            ERROR_THROW_IN(208, line, position);
        }
    }
}