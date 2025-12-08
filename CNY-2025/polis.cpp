#include "polis.h"
#include "semantic.h"   // нужно, чтобы вызвать SM::evaluateRPN
#include <stack>
#include <vector>
#include <iostream>
#include <cstring> // для strchr, если понадобится

using namespace std;

namespace PN
{
    int priority(char op)
    {
        switch (op)
        {
        case '(': return 0;
        case ')': return 0;
        case '=': return 1;
        case ',': return 2;
        case '+': case '-': return 3;
        case '*': case '/': case '%': return 4;
        default: return -1;
        }
    }

    bool polishNotation(int pos, LT::LexTable& lextable, IT::IdTable& idtable)
    {
        stack<LT::Entry> ops;   // стек операторов
        vector<LT::Entry> out;  // выходная строка (ОПН)
        stack<int> paramCount;  // стек счетчика параметров для функций

        // Находим конец выражения, чтобы знать границы перезаписи
        int exprEndIdx = pos;
        while (exprEndIdx < lextable.size) {
            char c = lextable.table[exprEndIdx].lexema;
            if (c == ';') { break; }
            exprEndIdx++;
        }

        bool lastLexemaWasOperand = false;

        for (int i = pos; i < exprEndIdx; ++i) {
            LT::Entry lex = lextable.table[i];
            char lx = lex.lexema;

            // ОПЕРАНДЫ (Идентификаторы и Литералы)
            if (lx == 'i' || lx == 'l') {
                bool isFuncCall = false;
                // если это имя функции, и за ним идет '(', то это начало вызова функции
                if (lx == 'i' && lex.idxIT != IT_NULLIDX) {
                    if (idtable.table[lex.idxIT].idtype == IT::IDTYPE::F) {
                        if (i + 1 < lextable.size && lextable.table[i + 1].lexema == '(') {
                            isFuncCall = true;
                        }
                    }
                }

                if (isFuncCall) {
                    ops.push(lex); // имя функции — в стек операторов (будем обрабатывать при ')')
                    lastLexemaWasOperand = false; // имя функции само по себе не операнд
                }
                else {
                    out.push_back(lex); // переменные и числа — сразу в выход
                    lastLexemaWasOperand = true;
                }
                continue;
            }

            // ОТКРЫВАЮЩАЯ СКОБКА
            if (lx == '(') {
                // определяем: является ли эта скобка аргументацией функции
                bool isFuncArgs = false;
                if (!ops.empty()) {
                    LT::Entry top = ops.top();
                    if (top.lexema == 'i' && top.idxIT != IT_NULLIDX) {
                        if (idtable.table[top.idxIT].idtype == IT::IDTYPE::F) {
                            isFuncArgs = true;
                        }
                    }
                }

                ops.push(lex);

                if (isFuncArgs) {
                    paramCount.push(0); // Начало подсчета параметров функции
                }
                else {
                    paramCount.push(-1); // Обычная скобка (не функция)
                }

                lastLexemaWasOperand = false;
                continue;
            }

            // ЗАКРЫВАЮЩАЯ СКОБКА
            if (lx == ')') {
                // выталкиваем всё до '('
                while (!ops.empty() && ops.top().lexema != '(') {
                    out.push_back(ops.top());
                    ops.pop();
                }

                // удаляем саму '('
                if (!ops.empty() && ops.top().lexema == '(') ops.pop();

                // обработка параметров функции
                if (!paramCount.empty()) {
                    int currentCount = paramCount.top();
                    paramCount.pop();

                    if (currentCount != -1) { // это был вызов функции
                        int finalArgCount = currentCount;
                        if (lastLexemaWasOperand) {
                            // если перед ')' был операнд, значит параметр не был учтён при запятых
                            finalArgCount++;
                        }

                        // в стеке сейчас должно лежать имя функции (оно должно быть над '(' в стеке ops)
                        if (!ops.empty() && ops.top().lexema == 'i' && ops.top().idxIT != IT_NULLIDX) {
                            LT::Entry funcName = ops.top();
                            ops.pop();

                            // 1. Добавляем количество аргументов (цифра или спецсимвол)
                            LT::Entry countLex;
                            countLex.sn = lex.sn; countLex.tn = lex.tn; countLex.idxIT = IT_NULLIDX;
                            // Для простоты, если аргументов <= 9, пишем цифру, иначе записываем '!' (можно изменить)
                            countLex.lexema = (finalArgCount <= 9) ? (char)('0' + finalArgCount) : '!';

                            out.push_back(countLex);

                            // 2. Добавляем оператор вызова '@'
                            LT::Entry callOp;
                            callOp.sn = lex.sn; callOp.tn = lex.tn; callOp.idxIT = IT_NULLIDX;
                            callOp.lexema = '@';
                            out.push_back(callOp);

                            // 3. Добавляем само имя функции (чтобы семантика могла найти её в idtable)
                            out.push_back(funcName);

                            // Результат вызова функции — это операнд
                            lastLexemaWasOperand = true;
                        }
                        else {
                            // Непредвиденная ситуация: имя функции не найдено в стеке ops.
                            lastLexemaWasOperand = true; // безопасный дефолт
                        }
                    }
                    else {
                        // обычные скобки (expression) -> выражение внутри скобок считается операндом
                        lastLexemaWasOperand = true;
                    }
                }
                continue;
            }

            // ЗАПЯТАЯ
            if (lx == ',') {
                // выталкиваем операторы до последней '('
                while (!ops.empty() && ops.top().lexema != '(') {
                    out.push_back(ops.top());
                    ops.pop();
                }
                // если мы внутри функции, увеличиваем счетчик параметров
                if (!paramCount.empty() && paramCount.top() != -1) {
                    int c = paramCount.top();
                    paramCount.pop();
                    paramCount.push(c + 1);
                }
                lastLexemaWasOperand = false; // Ждём следующий операнд
                continue;
            }

            // ОПЕРАТОРЫ (+, -, *, /, %, =, return)
            if (lx == '+' || lx == '-' || lx == '*' || lx == '/' || lx == '=' || lx == 'r') {
                // Для return ('r') и '=' приоритет низкий, они вытолкнут высокоприоритетные операции
                while (!ops.empty() && priority(ops.top().lexema) >= priority(lx)) {
                    out.push_back(ops.top());
                    ops.pop();
                }
                ops.push(lex);
                lastLexemaWasOperand = false;
                continue;
            }

            // Для других лексем — по умолчанию просто игнорируем или можно расширить
        }

        // выталкиваем оставшиеся операторы
        while (!ops.empty()) {
            if (ops.top().lexema == '(' || ops.top().lexema == ')') {
                // ошибка баланса скобок — удаляем лишнее
                ops.pop();
                continue;
            }
            out.push_back(ops.top());
            ops.pop();
        }

        // ПЕРЕЗАПИСЬ LexTable — записываем сформированную ОПН
        int outSize = (int)out.size();
        for (int k = 0; k < outSize; ++k) {
            if (pos + k < lextable.size) {
                lextable.table[pos + k] = out[k];
            }
            else {
                // Если выходная ОПН длиннее границ — это редкая ситуация; можно расширить таблицу
                // но сейчас просто игнорируем (или выбросить ошибку)
            }
        }

        // заполняем остаток выражения заглушками '#', чтобы сохранить структуру массива
        for (int k = pos + outSize; k < exprEndIdx; ++k) {
            lextable.table[k].lexema = '#';
            lextable.table[k].idxIT = IT_NULLIDX;
        }

        // --- ВЫЗОВ СЕМАНТИЧЕСКОЙ ОЦЕНКИ ОПН ---
        // Запускаем evaluateRPN, чтобы получить тип выражения и проверить вызовы функций/аргументы.
        // Если evaluateRPN возвращает UNDEF, значит семантика обнаружила проблему (или выражение некорректно).
        // В semantic.cpp функция должна быть объявлена в semantic.h как видимая (не static).
        IT::IDDATATYPE exprType = SM::evaluateRPN(pos, lextable, idtable);
        (void)exprType; // пока — не используем; можно логировать или действовать дальше

        return true;
    }
}
