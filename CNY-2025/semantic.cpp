#include "semantic.h"
#include "error.h"
#include <vector>
#include <string>
#include <iostream>
#include <stack>

using namespace std;

namespace SM {
    struct Context {
        IT::IDDATATYPE currentFuncType = IT::UNDEF;
        int braceDepth = 0;
    } context;

    string typeToString(IT::IDDATATYPE type) {
        switch (type) {
        case IT::INT: return "INT";
        case IT::STR: return "STR";
        default: return "UNKNOWN";
        }
    }

    string signToString(char type) {
        switch (type) {
        case LT::plus: return "+";
        case LT::minus: return "-";
        case LT::multiplication: return "*";
        case LT::division: return "/";
        case LT::increment: return "++";
        case LT::dicrement: return "--";
        case LT::inversion: return "~";
        default: return "UNKNOWN";
        }
    }

    void reportError(int code, int sn, int tn, const string& msg) {
        cerr << "Semantic Error [" << code << "] at line " << sn << ", token " << tn << ": " << msg << endl;
    }

    void checkFunctionCall(int funcItIdx, int openBracketPos, int& outCloseBracketPos, LT::LexTable& lextable, IT::IdTable& idtable);

    IT::IDDATATYPE analyzeExpression(int fromPos, int& outEndPos, LT::LexTable& lextable, IT::IdTable& idtable, bool isArgument = false) {
        IT::IDDATATYPE resultType = IT::UNDEF;
        int i = fromPos;

        if (i >= lextable.size) {
            outEndPos = i;
            return IT::UNDEF;
        }

        for (; i < lextable.size; ++i) {
            char lex = lextable.table[i].lexema;

            if (lex == ';' && !isArgument) {
                outEndPos = i;
                return resultType;
            }
            if (isArgument && (lex == ',' || lex == ')')) {
                outEndPos = i;
                return resultType;
            }
            if (!isArgument && lex == '}') {
                outEndPos = i;
                return resultType;
            }
            if (!isArgument && lex == '{') {
                outEndPos = i;
                return resultType;
            }
            if (!isArgument && lex == '.') {
                if (resultType != IT::INT || idtable.table[lextable.table[i + 1].idxIT].iddatatype != IT::INT) {
                    reportError(308, lextable.table[i].sn, lextable.table[i].tn, "Типы значений должны быть целыми числовыми.");
                }
                outEndPos = i;
                return resultType;
            }

            if (lex == LT_ID) {
                int idx = lextable.table[i].idxIT;
                if (idx == IT_NULLIDX) {
                    i++;
                    continue;
                }
                IT::Entry& entry = idtable.table[idx];

                if (!entry.isDefined) {
                    reportError(301, lextable.table[i].sn, lextable.table[i].tn, "Использование неопределенного идентификатора: " + entry.id);
                }

                if (i + 1 < lextable.size && lextable.table[i + 1].lexema == '(' && lextable.table[i].sign == LT::call) {
                    int closePos = 0;
                    checkFunctionCall(idx, i + 1, closePos, lextable, idtable);
                    resultType = entry.iddatatype;
                    i = closePos;
                    continue;
                }
                else {
                    if (entry.iddatatype != IT::UNDEF) {
                        if (resultType == IT::UNDEF) resultType = entry.iddatatype;
                        else if (resultType != entry.iddatatype) {
                            reportError(302, lextable.table[i].sn, lextable.table[i].tn,
                                "Несоответствие типов в выражении: " + typeToString(resultType) + " и " + typeToString(entry.iddatatype));
                            outEndPos = i;
                            return IT::UNDEF;
                        }
                    }
                }
            }

            // литерал
            else if (lex == 'l' || lex == LT_LITERAL) {
                if (lextable.table[i].idxIT != IT_NULLIDX) {
                    IT::IDDATATYPE litType = idtable.table[lextable.table[i].idxIT].iddatatype;
                    if (resultType == IT::UNDEF) resultType = litType;
                    else if (resultType != litType) {
                        reportError(302, lextable.table[i].sn, lextable.table[i].tn,
                            "Несоответствие типов в выражении: " + typeToString(resultType) + " и " + typeToString(litType));
                        outEndPos = i;
                        return IT::UNDEF;
                    }
                }
            }
            // скобки группировки
            else if (lex == '(') {
                int subEnd = 0;
                IT::IDDATATYPE subType = analyzeExpression(i + 1, subEnd, lextable, idtable, false);
                if (subType != IT::UNDEF) {
                    if (resultType == IT::UNDEF) resultType = subType;
                    else if (resultType != subType) {
                        reportError(302, lextable.table[i].sn, lextable.table[i].tn,
                            "Несоответствие типов в выражении (скобки): " + typeToString(resultType) + " и " + typeToString(subType));
                        outEndPos = subEnd;
                        return IT::UNDEF;
                    }
                }
                i = subEnd;
            }
            else if (lex == ')') {
                outEndPos = i;
                return resultType;
            }
            
            // операторы (+ - * / ++ -- ~)
            else if (lex == 'v' || lex == 'u') {
                if (resultType == IT::STR) {
                    reportError(308, lextable.table[i].sn, lextable.table[i].tn,
                        "Недопустимые операторы в выражении типа: " + typeToString(resultType) + " и " + signToString(lextable.table[i].sign));
                }
            }
        }

        outEndPos = i;
        return resultType;
    }


    void checkFunctionCall(int funcItIdx, int openBracketPos, int& outCloseBracketPos, LT::LexTable& lextable, IT::IdTable& idtable) {
        IT::Entry& funcEntry = idtable.table[funcItIdx];
        int expectedCount = funcEntry.params.count;
        int currentParamIndex = 0;

        int i = openBracketPos + 1;
        if (i >= lextable.size) {
            reportError(311, lextable.table[openBracketPos].sn, lextable.table[openBracketPos].tn, "Неожиданный конец файла после '('.");
            outCloseBracketPos = lextable.size - 1;
            return;
        }

        // пустой список аргументов
        if (lextable.table[i].lexema == ')') {
            outCloseBracketPos = i;
            if (expectedCount != 0) {
                reportError(303, lextable.table[i].sn, lextable.table[i].tn, "Ожидались параметры для функции " + funcEntry.id);
            }
            return;
        }

        // проходим аргументы
        while (i < lextable.size) {
            int endExprPos = 0;
            IT::IDDATATYPE exprType = analyzeExpression(i, endExprPos, lextable, idtable, true);

            if (endExprPos <= i) {
                int seek = i;
                while (seek < lextable.size && lextable.table[seek].lexema != ',' && lextable.table[seek].lexema != ')') seek++;
                if (seek >= lextable.size) {
                    reportError(312, lextable.table[lextable.size - 1].sn, lextable.table[lextable.size - 1].tn, "Неожиданный конец файла в аргументах.");
                    outCloseBracketPos = lextable.size - 1;
                    return;
                }
                endExprPos = seek;
            }

            // сравниваем с ожидаемым параметром
            if (currentParamIndex < expectedCount) {
                IT::IDDATATYPE expectedType = funcEntry.params.types[currentParamIndex];

                if (funcEntry.id == "print") { }
                else {
                    if (exprType == IT::UNDEF || exprType != IT::UNDEF && exprType != expectedType) {
                        reportError(304, lextable.table[i].sn, lextable.table[i].tn,
                            "Неверный тип аргумента " + to_string(currentParamIndex + 1) +
                            " в функции " + funcEntry.id + ". Ожидался: " + typeToString(expectedType) +
                            ", получен: " + typeToString(exprType));
                    }
                }
            }
            else {
                reportError(305, lextable.table[i].sn, lextable.table[i].tn, "Слишком много аргументов для функции " + funcEntry.id);
            }

            currentParamIndex++;

            i = endExprPos;

            if (i >= lextable.size) {
                reportError(312, lextable.table[lextable.size - 1].sn, lextable.table[lextable.size - 1].tn, "Неожиданный конец файла: ожидалась ')'");
                outCloseBracketPos = lextable.size - 1;
                return;
            }

            if (lextable.table[i].lexema == ')') {
                outCloseBracketPos = i;
                break;
            }
            else if (lextable.table[i].lexema == ',') {
                i++;
                continue;
            }
            else {
                reportError(310, lextable.table[i].sn, lextable.table[i].tn, "Ожидалась ',' или ')'");
                int seek = i;
                while (seek < lextable.size && lextable.table[seek].lexema != ')') seek++;
                outCloseBracketPos = (seek < lextable.size) ? seek : (lextable.size - 1);
                return;
            }
        }

        if (currentParamIndex < expectedCount) {
            reportError(306, lextable.table[outCloseBracketPos].sn, lextable.table[outCloseBracketPos].tn, "Недостаточно аргументов для функции " + funcEntry.id);
        }
    }


    void semanticAnalysis(LT::LexTable& lextable, IT::IdTable& idtable) {

        context.braceDepth = 0;
        context.currentFuncType = IT::UNDEF;

        for (int i = 0; i < lextable.size; ++i) {
            LT::Entry& lex = lextable.table[i];

            // --- function (Объявление) ---
            if (lex.lexema == 'f') {
                int funcIdx = -1;

                if (i + 2 < lextable.size && lextable.table[i + 2].lexema == 'i') {
                    funcIdx = lextable.table[i + 2].idxIT;
                    if (funcIdx != IT_NULLIDX) {
                        IT::Entry& funcEntry = idtable.table[funcIdx];
                        funcEntry.isDefined = true;
                        funcEntry.params.count = 0;
                        context.currentFuncType = funcEntry.iddatatype;
                        i += 3;
                        for (; i < lextable.size && lextable.table[i].lexema != ')'; ++i) {
                            if (lextable.table[i].sign == LT::parameter) {
                                idtable.table[lextable.table[i].idxIT].isDefined = true;
                                funcEntry.params.count++;
                                funcEntry.params.types.push_back(idtable.table[lextable.table[i].idxIT].iddatatype);
                            }
                            if (funcEntry.params.count > 3) {
                                reportError(310, lextable.table[i].sn, lextable.table[i].tn, "Превышено число допустимых параметров функции." + funcEntry.id);
                            }
                        }
                    }
                }

                int openBracePos = -1;
                for (int j = i + 1; j < lextable.size; ++j) {
                    if (lextable.table[j].lexema == '{') {
                        openBracePos = j;
                        break;
                    }
                }

                if (openBracePos != -1) {
                    context.braceDepth++;
                    i = openBracePos;
                }
                continue;
            }

            // --- объявление локальной переменной (type i) ---
            if (lex.lexema == 't') {
                if (i + 1 < lextable.size && lextable.table[i + 1].lexema == 'i') {
                    int varIdx = lextable.table[i + 1].idxIT;
                    if (varIdx != IT_NULLIDX) {
                        idtable.table[varIdx].isDefined = true;
                    }
                }
                continue;
            }

            // --- return ---
            else if (lex.lexema == 'r') {
                if (context.braceDepth == 0) {
                    reportError(308, lex.sn, lex.tn, "Оператор return вне функции.");
                }
                else {
                    int endExpr = 0;
                    IT::IDDATATYPE retType = analyzeExpression(i + 1, endExpr, lextable, idtable, false);

                    if (context.currentFuncType != IT::UNDEF && retType != IT::UNDEF && retType != context.currentFuncType) {
                        reportError(309, lex.sn, lex.tn,
                            "Тип возвращаемого значения " + typeToString(retType) +
                            " не соответствует типу функции " + typeToString(context.currentFuncType));
                    }
                    i = endExpr;
                }
                continue;
            }

            // --- Открывающая фигурная скобка ---
            else if (lex.lexema == '{') {
                context.braceDepth++;
                continue;
            }

            // --- закрывающая фигурная скобка ---
            else if (lex.lexema == '}') {
                if (context.braceDepth > 0) {
                    context.braceDepth--;
                    if (context.braceDepth == 0) {
                        context.currentFuncType = IT::UNDEF;
                    }
                }
                continue;
            }

            // --- вызов функции (в виде инструкции) или присваивание ---
            if (lex.lexema == LT_ID && lex.idxIT != IT_NULLIDX) {
                IT::Entry& entry = idtable.table[lex.idxIT];

                if ((lex.sign == LT::call || lex.sign == LT::print || lex.sign == LT::date || lex.sign == LT::time)
                    && i + 1 < lextable.size && lextable.table[i + 1].lexema == '(') {
                    int closePos = 0;
                    checkFunctionCall(lex.idxIT, i + 1, closePos, lextable, idtable);
                    i = closePos;
                    if (i + 1 < lextable.size && lextable.table[i + 1].lexema == ';') i++;
                    continue;
                }

                // присваивание: идентификатор '=' выражение
                if (entry.idtype == IT::V || entry.idtype == IT::P) {
                    if (!entry.isDefined) {
                        reportError(301, lex.sn, lex.tn, "Переменная '" + entry.id + "' не определена.");
                    }

                    if (i + 1 < lextable.size && lextable.table[i + 1].lexema == '=') {
                        int endExpr = 0;
                        IT::IDDATATYPE rhsType = analyzeExpression(i + 2, endExpr, lextable, idtable, false);

                        if (rhsType != IT::UNDEF && rhsType != entry.iddatatype) {
                            reportError(307, lex.sn, lex.tn,
                                "Невозможно присвоить " + typeToString(rhsType) + " переменной типа " + typeToString(entry.iddatatype));
                        }

                        i = endExpr;
                        continue;
                    }
                }
            }
            // прочие токены — пропускаем
        }
    }

}
