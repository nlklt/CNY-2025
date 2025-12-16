#include "polis.h"
#include "error.h"
#include <stack>
#include <vector>
#include <iostream>

using namespace std;

namespace PN
{
    int priority(LT::Entry lex)
    {
        switch (lex.sign)
        {
        case LT_LEFTHESIS:  return 0;
        case LT_RIGHTHESIS: return 0;

        case LT_COMMA: return 2;

        case LT::SIGNATURE::plus:  return 3;
        case LT::SIGNATURE::minus: return 3;

        case LT::SIGNATURE::multiplication: return 4;
        case LT::SIGNATURE::division:       return 4;

        case LT::SIGNATURE::increment:       return 5;
        case LT::SIGNATURE::dicrement:       return 5;
        case LT::SIGNATURE::inversion:       return 5;
        default: return -1;
        }
    }

    bool polishNotation(int pos, LT::LexTable& lextable, IT::IdTable& idtable)
    {
        stack<LT::Entry> ops;            // стек операторов и имён функций
        vector<LT::Entry> out;           // выход (ОПН)
        stack<int> paramCount;           // стек счётчиков аргументов для функций

        // находим конец выражения (';')
        int exprEndIdx = pos;
        while (exprEndIdx < lextable.size && lextable.table[exprEndIdx].lexema != ';') ++exprEndIdx;

        bool lastLexemaWasOperand = false;

        for (int i = pos; i < exprEndIdx; ++i) {
            LT::Entry lex = lextable.table[i];
            char lx = lex.lexema;

            // --- операнд: идентификатор или литерал
            if (lx == 'i' || lx == 'l') {
                bool nextIsLeftParen = (i + 1 < exprEndIdx && lextable.table[i + 1].lexema == '(');
                bool isFuncName = false;
                if (lx == 'i' && lex.idxIT != IT_NULLIDX && nextIsLeftParen) {
                    if (idtable.table[lex.idxIT].idtype == IT::IDTYPE::F) isFuncName = true;
                }

                if (isFuncName) {
                    ops.push(lex); // имя функции во стек
                    lastLexemaWasOperand = false;
                }
                else {
                    out.push_back(lex);
                    lastLexemaWasOperand = true;
                }
                continue;
            }

            // --- открывающая скобка
            if (lx == '(') {
                bool isFuncArgs = (!ops.empty() && ops.top().lexema == 'i' && ops.top().idxIT != IT_NULLIDX
                    && idtable.table[ops.top().idxIT].idtype == IT::IDTYPE::F);

                ops.push(lex);
                if (isFuncArgs) paramCount.push(0);
                else paramCount.push(-1);

                lastLexemaWasOperand = false;
                continue;
            }

            // --- закрывающая скобка
            if (lx == ')') {
                while (!ops.empty() && ops.top().lexema != '(') {
                    out.push_back(ops.top()); ops.pop();
                }

                if (!ops.empty() && ops.top().lexema == '(') {
                    ops.pop();
                }

                if (!paramCount.empty()) {
                    int cnt = paramCount.top(); paramCount.pop();

                    if (cnt != -1) {
                        // аргументация функции: если последний был операнд, учитываем его
                        int finalArgCount = cnt + (lastLexemaWasOperand ? 1 : 0);

                        // над '(' в стеке должно быть имя функции
                        if (!ops.empty() && ops.top().lexema == 'i' && ops.top().idxIT != IT_NULLIDX
                            && idtable.table[ops.top().idxIT].idtype == IT::IDTYPE::F) {

                            LT::Entry funcName = ops.top(); ops.pop();

                            LT::Entry callOp = lex;
                            callOp.lexema = '@';
                            callOp.idxIT = IT_NULLIDX;

                            out.push_back(callOp);
                            out.push_back(funcName);

                            lastLexemaWasOperand = true;
                        }
                        else {
                            // имя функции не найдено — считаем, что выражение внутри скобок — операнд
                            lastLexemaWasOperand = true;
                        }
                    }
                    else {
                        // обычные скобки — выражение внутри считается операндом
                        lastLexemaWasOperand = true;
                    }
                }
                continue;
            }

            // --- запятая (разделитель параметров)
            if (lx == ',') {
                while (!ops.empty() && ops.top().lexema != '(') { out.push_back(ops.top()); ops.pop(); }
                if (!paramCount.empty() && paramCount.top() != -1) {
                    int c = paramCount.top(); paramCount.pop(); paramCount.push(c + 1);
                }
                lastLexemaWasOperand = false;
                continue;
            }

            int pcur = priority(lex);
            // --- операторы: унарные, определить рассположение
            if (lx == 'u') {
                switch (lex.sign) {
                case LT::SIGNATURE::increment:
                    if (lastLexemaWasOperand) {
                        lex.sign = LT::SIGNATURE::increment_post;
                    }
                    else {
                        lex.sign = LT::SIGNATURE::pref_increment;
                    }
                    break;
                case LT::SIGNATURE::dicrement:
                    if (lastLexemaWasOperand) {
                        lex.sign = LT::SIGNATURE::dicrement_post;
                    }
                    else {
                        lex.sign = LT::SIGNATURE::pref_dicrement;
                    }
                    break;
                case LT::SIGNATURE::inversion:
                    if (lastLexemaWasOperand) {
                        lex.sign = LT::SIGNATURE::inversion_post;
                    }
                    else {
                        lex.sign = LT::SIGNATURE::pref_inversion;
                    }
                    break;
                }
                out.push_back(lex);
                continue;
            }

            // --- операторы: определим наличие оператора по приоритету
            if (pcur > 0) {
                // выталкиваем из стека все операторы с приоритетом >= текущего (левоассоциативность)
                while (!ops.empty() && ops.top().lexema != '(') {
                    int ptop = priority(ops.top());
                    if (ptop < 0) break; // не оператор
                    if (ptop >= pcur) { out.push_back(ops.top()); ops.pop(); }
                    else break;
                }
                ops.push(lex);
                lastLexemaWasOperand = false;
                continue;
            }
        }

        // выталкиваем оставшиеся операторы
        while (!ops.empty()) {
            if (ops.top().lexema == '(' || ops.top().lexema == ')') { ops.pop(); continue; }
            out.push_back(ops.top()); ops.pop();
        }

        // проверка размеров и копирование результата обратно в lextable
        int outSize = (int)out.size();
        // проверка, что мы не выйдем за пределы
        if (pos + outSize > lextable.size) ERROR_THROW(102);

        for (int k = 0; k < outSize; ++k) {
            // перезаписываем ячейку полностью (чтобы не осталось древних полей)
            lextable.table[pos + k] = out[k];
        }

        // очищаем следующее пространство до ';'
        for (int k = pos + outSize; k < exprEndIdx; ++k) {
            lextable.table[k].lexema = '#';
            lextable.table[k].idxIT = IT_NULLIDX;
            lextable.table[k].sign = 0;
            // обнуляем другие поля по необходимости (value, line, tn ...)
        }

        return true;
    }
}

