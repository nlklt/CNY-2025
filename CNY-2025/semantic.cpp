#include "semantic.h"
#include "polis.h"
#include "error.h"

#include <stack>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

using namespace std;

namespace SM
{
    using LTEntry = LT::Entry;
    using ITEntry = IT::Entry;

    static void reportError(int code, int sn, int tn, const string& message)
    {
        cerr << "Semantic error [" << code << "] at sn=" << sn << " tn=" << tn << ": " << message << '\n';
    }

    static bool areTypesCompatible(IT::IDDATATYPE target, IT::IDDATATYPE source)
    {
        if (target == source) return true;
        return false;
    }

    IT::IDDATATYPE getTypeFromLexEntry(const LT::Entry& lex, IT::IdTable& idtable)
    {
        if (lex.idxIT != IT_NULLIDX && lex.idxIT >= 0 && lex.idxIT < (int)idtable.size)
        {
            return idtable.table[lex.idxIT].iddatatype;
        }
        return IT::IDDATATYPE::UNDEF;
    }

    static vector<IT::IDDATATYPE> getFunctionParamTypes(LTEntry lex, IT::IdTable& idtable)
    {
        vector<IT::IDDATATYPE> res;
        for (int k = lex.idxIT; k < (int)idtable.size && idtable.table[k].idtype == IT::IDTYPE::P; k++)
        {
            if (lex.idxIT >= 0 && lex.idxIT <= (int)idtable.size && lex.idxIT != IT_NULLIDX) {
                const ITEntry& e = idtable.table[k];
                res.push_back(e.iddatatype);
            }
        }
        return res;
    }

    void semanticAnalysis(LT::LexTable& lextable, IT::IdTable& idtable)
    {
        vector<int> funcStack;

        bool isExpression = false;
        int posExpression = 0;
        for (int i = 0; i < lextable.size; ++i)
        {
            LTEntry& lex = lextable.table[i];
            char lx = lex.lexema;
            
            if (lx == '=')
            {
                isExpression = true;
                posExpression = i + 1;
                if (i - 1 >= 0 && lextable.table[i - 1].idxIT != IT_NULLIDX)
                {
                    int lhsIdx = lextable.table[i - 1].idxIT;
                    ITEntry& lhsEntry = idtable.table[lhsIdx];

                    bool isDeclaration = (i - 2 >= 0 && lextable.table[i - 2].lexema == 't');
                    if (isDeclaration) {
                        lhsEntry.isDefined = true;
                    }
                    else if (!lhsEntry.isDefined) {
                        reportError(777, lextable.table[i - 1].sn, lextable.table[i - 1].tn, "Использование идентификатора до определения/инициализации.");
                    }

                    int exprPos = i + 1;
                }
                continue;
            }


            // Проверка использования переменных
            if (lx == 'i' && lex.idxIT != IT_NULLIDX)
            {
                int id = lex.idxIT;
                ITEntry& e = idtable.table[id];

                if (e.idtype == IT::IDTYPE::V) {
                    bool isDecl = (i - 1 >= 0 && lextable.table[i - 1].lexema == 't');
                    bool isLHS = (i + 1 < lextable.size && lextable.table[i + 1].lexema == '=');

                    if (isDecl) {
                        idtable.table[lextable.table[i].idxIT].isDefined = true;
                    }
                    if (!isDecl && !isLHS && !e.isDefined) {
                        reportError(777, lex.sn, lex.tn, "Использование переменной до определения.");
                    }
                }

                else if (e.idtype == IT::IDTYPE::F) {
                    if (i - 2 >= 0 && lextable.table[i - 2].lexema == 'f') {
                        e.idtype = IT::IDTYPE::C;
                        e.isDefined = true;
                    }
                    bool isDecl = (i - 1 >= 0 && lextable.table[i - 1].lexema == 't');
                    bool isLHS = (i + 1 < lextable.size && lextable.table[i + 1].lexema == '(');

                    std::vector<IT::IDDATATYPE> typesOfPrarams = getFunctionParamTypes(lextable.table[i + 3], idtable);

                    idtable.table[lex.idxIT].params.count = typesOfPrarams.size();
                    idtable.table[lex.idxIT].params.types = typesOfPrarams;

                    if (isDecl) {
                        idtable.table[lextable.table[i].idxIT].isDefined = true;
                    }
                    if (!isDecl && !isLHS && !e.isDefined) {
                        reportError(777, lex.sn, lex.tn, "Использование переменной до определения.");
                    }
                }
                else if (e.idtype == IT::IDTYPE::C) {
                    if (e.isDefined) {
                        int k = lex.idxIT;
                        int t = 0;
                        ++i;
                        while (lextable.table[i].lexema != LT_RIGHTHESIS) {
                            if (lextable.table[i].idxIT >= 0 && lextable.table[i].idxIT <= (int)idtable.size && lextable.table[i].idxIT != IT_NULLIDX) {
                                const ITEntry& e = idtable.table[k];
                                if (idtable.table[lextable.table[i].idxIT].iddatatype == idtable.table[k].params.types[t]) {
                                    ++i;
                                    ++t;
                                    continue;
                                }
                                else {
                                    break;
                                    ++i;
                                }
                            }
                            else {
                                ++i;
                            }
                        }
                        if (t == idtable.table[lex.idxIT].params.count) {
                            continue;
                        }
                        else {
                            reportError(777, lex.sn, lex.tn, "Не соответствие аргументов функции.");
                        }
                    }
                    else {
                        reportError(777, lex.sn, lex.tn, "Функция с таким именем не объявлена.");
                    }
                }
                else if (e.idtype != IT::IDTYPE::P){
                   reportError(777, lex.sn, lex.tn, "Неопределенный идентификатор.");
                }
            }
        }
    }
}
