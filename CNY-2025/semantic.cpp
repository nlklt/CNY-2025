#include "semantic.h"
#include "polis.h"
#include "error.h"

#include <stack>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

//	namespace SM
//	{
//		IT::IDDATATYPE getType(int i, LT::LexTable& lextable, IT::IdTable& idtable) {
//			switch (lextable.table[i].sign) {
//			case (LT::SIGNATURE::t_int): return IT::IDDATATYPE::INT;
//			case (LT::SIGNATURE::t_char): return IT::IDDATATYPE::CHR;
//			case (LT::SIGNATURE::t_string): return IT::IDDATATYPE::STR;
//			default :
//				if (lextable.table[i].idxIT != IT_NULLIDX) {
//					return (idtable.table[lextable.table[i].idxIT].iddatatype);
//				}
//			}
//		}
//		void semanticAnalysis(LT::LexTable& lextable, IT::IdTable& idtable)
//		{
//			IT::IDDATATYPE currIdType = IT::IDDATATYPE::UNDEF;
//			IT::IDDATATYPE currReturnType = IT::IDDATATYPE::NONE;
//			for (int i = 0; i < lextable.size; i++)
//			{
//				switch (lextable.table[i].lexema) {
//					if (lextable.table[i].idxIT != IT_NULLIDX) {
//				case LT_ID:
//					// объявление / определение переменной
//					if (currIdType != IT::IDDATATYPE::UNDEF) {
//						idtable.table[lextable.table[i].idxIT].isDefined = true;
//					}
//					// проверка идентификатора
//					else if (idtable.table[lextable.table[i].idxIT].isDefined == false) {
//						cerr << "Необъявленный идентификатор\n";
//					}
//					// объявление функции
//					if (idtable.table[lextable.table[i].idxIT].idtype == IT::IDTYPE::F) {
//						// polishNotation function int fnm(int a, int b, int c) -> ft i(параметр)i(параметр)i(параметр)@3i(функция)
//						for (int k = i + 1; k < lextable.size; k++) {
//							if (lextable.table[k].lexema == LT_LEFTHESIS) {
//								continue;
//							}
//							if (lextable.table[k].lexema == LT_ID) {
//								
//							}
//							else {
//								cerr << "!!!!!!!!!!!!!_1114_!!!!!!!!!!!!!\n";
//							}
//							if (lextable.table[k].lexema == LT_FUNCTION || lextable.table[k].lexema == LT_MAIN) break;
//						}
//					}
//					break;
//					}
//					else {
//				case LT_TYPE:
//					currIdType = getType(i, lextable, idtable);
//				case LT_FUNCTION:
//					currReturnType = getType(i + 1, lextable, idtable);
//				case LT_RETURN:
//					// polishNotation return x; -> r x; | return (x / 5); -> r x(операнд)5(операнд)/(действие); 
//					while (lextable.table[++i].lexema != LT_SEMICOLON) {
//						if (lextable.table[i].idxIT != IT_NULLIDX) {
//							if (getType(i, lextable, idtable) == currReturnType) {
//								continue;
//							}
//							else
//							{
//								ERROR_THROW(777, lextable.table[i].sn, lextable.table[i].tn);
//							}
//						}
//					}
//				case LT_EQUAL:
//					// polishNotation x = (a + 4); -> x = i(операнд)i(операнд)+(действие);

// semantic.cpp
#include "semantic.h"
#include "polis.h"
// #include "error.h"  // больше не используем ERROR_THROW

#include <stack>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

#include "semantic.h"
#include "polis.h"
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

    static IT::IDDATATYPE getTypeFromLexEntry(const LTEntry& lex, IT::IdTable& idtable)
    {
        if (lex.idxIT != IT_NULLIDX && lex.idxIT >= 0 && lex.idxIT < (int)idtable.size)
        {
            return idtable.table[lex.idxIT].iddatatype;
        }
        return IT::IDDATATYPE::UNDEF;
    }

    static vector<IT::IDDATATYPE> getFunctionParamTypes(int funcIdx, IT::IdTable& idtable)
    {
        vector<IT::IDDATATYPE> res;
        if (funcIdx < 0 || funcIdx >= (int)idtable.size) return res;
        string funcName = idtable.table[funcIdx].id;
        for (int k = 0; k < (int)idtable.size; ++k)
        {
            const ITEntry& e = idtable.table[k];
            if (e.idtype == IT::IDTYPE::P && e.scope == funcName)
            {
                res.push_back(e.iddatatype);
            }
        }
        return res;
    }

    static IT::IDDATATYPE evaluateRPN(int pos, LT::LexTable& lextable, IT::IdTable& idtable)
    {
        vector<LTEntry> rpn;
        for (int j = pos; j < lextable.size; ++j)
        {
            char lx = lextable.table[j].lexema;
            if (lx == ';' || lx == '#') break;
            rpn.push_back(lextable.table[j]);
        }

        stack<IT::IDDATATYPE> st;
        for (size_t i = 0; i < rpn.size(); ++i)
        {
            LTEntry& lex = rpn[i];
            char lx = lex.lexema;

            if (lx == 'i' || lx == 'l') // Идентификатор или Литерал
            {
                IT::IDDATATYPE t = getTypeFromLexEntry(lex, idtable);
                st.push(t);
                continue;
            }
            if (lx == '@')
            {
                if (i + 1 >= rpn.size()) return IT::IDDATATYPE::UNDEF;
                LTEntry funcLex = rpn[i + 1];
                int funcIdx = funcLex.idxIT;

                IT::IDDATATYPE retType = IT::IDDATATYPE::UNDEF;
                if (funcIdx != IT_NULLIDX) {
                    retType = idtable.table[funcIdx].iddatatype;
                    }
                st.push(retType);
                ++i;
                continue;
            }
            if (strchr("+-*/", lx)) // Операторы
            {
                if (st.size() < 2) return IT::IDDATATYPE::UNDEF;
                IT::IDDATATYPE r = st.top(); st.pop();
                IT::IDDATATYPE l = st.top(); st.pop();
                // Логика типов операций
                st.push(IT::IDDATATYPE::INT);
            }
        }
        return st.empty() ? IT::IDDATATYPE::UNDEF : st.top();
    }

    void semanticAnalysis(LT::LexTable& lextable, IT::IdTable& idtable)
    {
        vector<int> funcStack;

        for (int i = 0; i < lextable.size; ++i)
        {
            LTEntry& lex = lextable.table[i];
            char lx = lex.lexema;

            if (lx == 'f') {
                if (lextable.table[i + 2].idxIT != IT_NULLIDX) {
                    int idx = lextable.table[i + 2].idxIT;
                    idtable.table[idx].params.types = getFunctionParamTypes(idx, idtable);
                    idtable.table[idx].params.count = idtable.table[idx].params.types.size();
                    if (idtable.table[idx].idtype == IT::IDTYPE::F) {
                        int foundFuncIdx = idx;
                        funcStack.push_back(foundFuncIdx);
                        break;
                    }
                }
            }
            if (lx == '{')
            {
                int foundFuncIdx = -1;
                for (int b = i - 1; b >= 0 && (i - b) < 5; --b) {
                    if (lextable.table[b].lexema == 'm') {
                        funcStack.push_back(-1);
                        break;
                    }
                    if (lextable.table[b].idxIT != IT_NULLIDX) {
                        int idx = lextable.table[b].idxIT;
                        if (idtable.table[idx].idtype == IT::IDTYPE::F) {
                            foundFuncIdx = idx;
                            funcStack.push_back(foundFuncIdx);
                            break;
                        }
                    }
                    if (lextable.table[b].lexema == LT_FOR) {
                        funcStack.push_back(1);
                        break;
                    }
                }
                continue;
            }

            // --- ВЫХОД ИЗ ФУНКЦИИ ---
            if (lx == '}')
            {
                if (!funcStack.empty()) funcStack.pop_back();
                continue;
            }

            // --- ПРИСВАИВАНИЕ / ИНИЦИАЛИЗАЦИЯ ---
            if (lx == '=')
            {
                if (i - 1 >= 0 && lextable.table[i - 1].idxIT != IT_NULLIDX)
                {
                    int lhsIdx = lextable.table[i - 1].idxIT;
                    ITEntry& lhsEntry = idtable.table[lhsIdx];

                    // FIX: Проверяем, является ли это инициализацией (перед переменной стоит тип 't')
                    bool isDeclaration = (i - 2 >= 0 && lextable.table[i - 2].lexema == 't');

                    // Ошибку кидаем только если это НЕ декларация и переменная НЕ определена
                    if (!isDeclaration && !lhsEntry.isDefined && lhsEntry.idtype == IT::IDTYPE::V) {
                        reportError(777, lextable.table[i - 1].sn, lextable.table[i - 1].tn, "Использование переменной до определения/инициализации.");
                    }

                    // Вычисляем выражение справа
                    int exprPos = i + 1;
                    PN::polishNotation(exprPos, lextable, idtable);
                    IT::IDDATATYPE exprType = evaluateRPN(exprPos, lextable, idtable);

                    if (exprType != IT::IDDATATYPE::UNDEF) {
                        if (!areTypesCompatible(lhsEntry.iddatatype, exprType)) {
                            reportError(777, lex.sn, lex.tn, "Несовместимость типов при присваивании.");
                        }
                    }

                    // Помечаем как определенную ПОСЛЕ присваивания
                    lhsEntry.isDefined = true;
                }
                continue;
            }

            // --- RETURN ---
            if (lx == 'r')
            {
                if (funcStack.empty()) {
                    reportError(777, lex.sn, lex.tn, "return вне функции.");
                    continue;
                }
                // Проверку типа возврата можно делать тут
                continue;
            }

            // --- ИСПОЛЬЗОВАНИЕ ПЕРЕМЕННОЙ ---
            if (lx == 'i' && lex.idxIT != IT_NULLIDX)
            {
                int id = lex.idxIT;
                ITEntry& e = idtable.table[id];

                if (e.idtype == IT::IDTYPE::V) {
                    // FIX: Игнорируем проверку, если:
                    // 1. Это место декларации (перед ID стоит тип 't')
                    // 2. Это левая часть присваивания (после ID стоит '=') - проверка будет в блоке '='
                    bool isDecl = (i - 1 >= 0 && lextable.table[i - 1].lexema == 't');
                    bool isLHS = (i + 1 < lextable.size && lextable.table[i + 1].lexema == '=');

                    if (!isDecl && !isLHS && !e.isDefined) {
                        reportError(777, lex.sn, lex.tn, "Использование переменной до определения.");
                    }
                    if (isDecl) {
                        idtable.table[lextable.table[i].idxIT].isDefined = true;
                    }
                }
            }
        }
    }
}