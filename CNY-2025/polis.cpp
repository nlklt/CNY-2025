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
        // Не кладём начальный 0 — создаём запись для скобок только при встрече '('

        // находим конец выражения (';')
        int exprEndIdx = pos;
        while (exprEndIdx < lextable.size && lextable.table[exprEndIdx].lexema != ';') ++exprEndIdx;

        bool lastLexemaWasOperand = false;

        for (int i = pos; i < exprEndIdx; ++i) {
            LT::Entry lex = lextable.table[i];
            char lx = lex.lexema;

            // идентификатор или литерал
            if (lx == 'i' || lx == 'l') {
                // если след. лексема — '(', то это возможно имя функции; смотрим в idtable
                bool nextIsLeftParen = (i + 1 < exprEndIdx && lextable.table[i + 1].lexema == '(');
                bool isFuncName = false;
                if (lx == 'i' && lex.idxIT != IT_NULLIDX && nextIsLeftParen) {
                    if (idtable.table[lex.idxIT].idtype == IT::IDTYPE::C) isFuncName = true;
                }

                if (isFuncName) {
                    ops.push(lex); // имя функции во стек (будет обработано при ')')
                    lastLexemaWasOperand = false;
                }
                else {
                    out.push_back(lex);
                    lastLexemaWasOperand = true;
                }
                continue;
            }

            // открывающая скобка
            if (lx == '(') {
                // определяем, является ли скобка аргументацией функции: если над ней в стеке лежит имя функции
                bool isFuncArgs = (!ops.empty() && ops.top().lexema == 'i' && ops.top().idxIT != IT_NULLIDX
                    && idtable.table[ops.top().idxIT].idtype == IT::IDTYPE::C);

                ops.push(lex);

                if (isFuncArgs)
                    paramCount.push(0); // начинаем считать аргументы для функции
                else
                    paramCount.push(-1); // обычные скобки

                lastLexemaWasOperand = false;
                continue;
            }

            // закрывающая скобка
            if (lx == ')') {
                // выталкиваем операторы до '('
                while (!ops.empty() && ops.top().lexema != '(') {
                    out.push_back(ops.top()); ops.pop();
                }

                // удаляем '('
                if (!ops.empty() && ops.top().lexema == '(') ops.pop();

                if (!paramCount.empty()) {
                    int cnt = paramCount.top(); paramCount.pop();

                    if (cnt != -1) {
                        // это была аргументация функции
                        int finalArgCount = cnt + (lastLexemaWasOperand ? 1 : 0);

                        // над '(' в стеке должно быть имя функции
                        if (!ops.empty() && ops.top().lexema == 'i' && ops.top().idxIT != IT_NULLIDX
                            && idtable.table[ops.top().idxIT].idtype == IT::IDTYPE::C) {

                            LT::Entry funcName = ops.top(); ops.pop();

                            // запись: сначала количество аргументов, затем операция вызова '@', затем имя функции
                            LT::Entry countLex = lex;
                            countLex.idxIT = IT_NULLIDX;
                            // ограничимся 0..9 символом, >9 пометим как '!' (можно изменить на другой подход)
                            countLex.lexema = (finalArgCount <= 9) ? (char)('0' + finalArgCount) : '!';
                            out.push_back(countLex);

                            LT::Entry callOp = lex; callOp.idxIT = IT_NULLIDX; callOp.lexema = '@';
                            out.push_back(callOp);

                            out.push_back(funcName);

                            lastLexemaWasOperand = true;
                        }
                        else {
                            // имя функции не найдено — безопасно считаем результат операндом
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

            // запятая — разделитель параметров
            if (lx == ',') {
                // выталкиваем операторы до ближайшей '('
                while (!ops.empty() && ops.top().lexema != '(') { out.push_back(ops.top()); ops.pop(); }
                // если текущие скобки — аргументы функции, увеличиваем счётчик
                if (!paramCount.empty() && paramCount.top() != -1) {
                    int c = paramCount.top(); paramCount.pop(); paramCount.push(c + 1);
                }
                lastLexemaWasOperand = false;
                continue;
            }

            // операторы (универсальная обработка бинарных/унарных)
            if (lx == 'v' || lx == 'u' || lx == '+' || lx == '-' || lx == '*' || lx == '/') {
                // Учет приоритета: если верх стека имеет приоритет >= текущего и это не '(', выталкиваем
                while (!ops.empty() && ops.top().lexema != '(' && priority(ops.top()) >= priority(lex)) {
                    out.push_back(ops.top()); ops.pop();
                }
                ops.push(lex);
                lastLexemaWasOperand = false;
                continue;
            }

            // прочие — игнорируем
        }

        // выталкиваем оставшиеся операторы
        while (!ops.empty()) {
            if (ops.top().lexema == '(' || ops.top().lexema == ')') { ops.pop(); continue; }
            out.push_back(ops.top()); ops.pop();
        }

        // перезапись lextable (проверяем границы)
        int outSize = (int)out.size();
        for (int k = 0; k < outSize; ++k) {
            if (pos + k < lextable.size) lextable.table[pos + k] = out[k];
            else ERROR_THROW(102);
        }

        // очищаем следующее пространство до ';'
        for (int k = pos + outSize; k < exprEndIdx; ++k) {
            lextable.table[k].lexema = '#';
            lextable.table[k].idxIT = IT_NULLIDX;
            // при необходимости обнулять и другие поля (sign, value и т.д.)
        }

        return true;
    }
}
