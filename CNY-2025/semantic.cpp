//#include "semantic.h"
//#include "error.h"
//#include <iostream>
//
//using namespace std;
//
//namespace Semantic
//{
//	// Вспомогательная функция для получения типа данных из ID или Литерала
//	IT::IDDATATYPE getDataType(LT::Entry& lex, IT::IdTable& idtable)
//	{
//		if (lex.lexema == LT_LITERAL || lex.lexema == LT_ID)
//		{
//			return idtable.table[lex.idxIT].iddatatype;
//		}
//		return IT::IDDATATYPE::UNDEF;
//	}
//		void semanticAnalysis(LT::LexTable& lextable, IT::IdTable& idtable)
//		{
//			// 1. main
//			bool hasMain = false;
//			for (int i = 0; i < idtable.size; i++)
//			{
//				if (lextable.table[i].lexema == LT_MAIN)
//				{
//					hasMain = true;
//					break;
//				}
//			}
//			if (!hasMain) ERROR_THROW(706);
//
//			// контекст
//			IT::IDDATATYPE currentFuncType = IT::IDDATATYPE::UNDEF;
//			bool insideFunction = false;
//			int braceDepth = 0;
//
//			// Проход по таблице лексем
//			for (int i = 0; i < lextable.size; i++)
//			{
//				LT::Entry& lexE = lextable.table[i];
//				IT::Entry* idE = (lexE.idxIT != LT_NULLIDX) ? &idtable.table[lexE.idxIT] : nullptr;
//
//				switch (lexE.lexema)
//				{
//					// объявление ф-ии: fti (ti, ti) { }
//				case LT_FUNCTION:
//				{
//					if (i + 2 < lextable.size)
//					{
//						LT::Entry& functionId = lextable.table[i + 2];
//						if (functionId.lexema == LT_ID)
//						{
//							currentFuncType = idtable.table[functionId.idxIT].iddatatype;
//							insideFunction = true;
//						}
//					}
//					break;
//				}
//
//				case LT_LEFTBRACE:
//				{
//					if (insideFunction) braceDepth++;
//					break;
//				}
//				case LT_RIGHTBRACE:
//				{
//					if (insideFunction)
//					{
//						braceDepth--;
//						if (braceDepth == 0)
//						{
//							insideFunction = false;
//							currentFuncType = IT::IDDATATYPE::UNDEF;
//						}
//					}
//					break;
//				}
//
//				// цикл: c ti = l.l { }
//				/*
//				for char i
//				'0'..9
//				3..1
//				*/
//				case LT_FOR:
//				{
//					LT::Entry& iterator = lexE; // итератор
//
//					// 2. Проверяем границы диапазона
//					IT::IDDATATYPE startType = getDataType(iterator, idtable);
//					IT::IDDATATYPE endType = getDataType(iterator, idtable);
//
//					// Ожидаем INT
//					if (startType != IT::IDDATATYPE::INT || endType != IT::IDDATATYPE::INT)
//					{
//						ERROR_THROW_IN(708, lexE.sn, -1); // Ошибка: границы цикла должны быть целочисленными
//					}
//					break;
//				}
//
//				// --- Операции (деление на 0) ---
//				case LT_OPERATION:
//				{
//					if (lexE.sign == LT::SIGNATURE::division) // '/'
//					{
//						if (i + 1 < lextable.size)
//						{
//							LT::Entry& nextLex = lextable.table[i + 1];
//							if (nextLex.lexema == LT_LITERAL)
//							{
//								IT::Entry& lit = idtable.table[nextLex.idxIT];
//								// Проверка на 0
//								if (lit.iddatatype == IT::IDDATATYPE::INT && lit.value.vint == 0)
//									ERROR_THROW_IN(700, nextLex.sn, nextLex.tn);
//							}
//						}
//					}
//					break;
//				}
//
//				// --- Присваивание ---
//				case LT_EQUAL:
//				{
//					// Слева должен быть ID (lexE[i-1])
//					if (i > 0 && lextable.table[i - 1].lexema == LT_ID)
//					{
//						IT::IDDATATYPE targetType = idtable.table[lextable.table[i - 1].idxIT].iddatatype;
//
//						// Проходим вправо до точки с запятой
//						bool ignore = false;
//						for (int k = i + 1; k < lextable.size; k++)
//						{
//							LT::Entry& rhsLex = lextable.table[k];
//							if (rhsLex.lexema == LT_SEMICOLON) break;
//
//							// Пропуск параметров функций внутри выражения
//							if (rhsLex.lexema == LT_LEFTHESIS) { ignore = true; continue; }
//							if (rhsLex.lexema == LT_RIGHTHESIS) { ignore = false; continue; }
//							if (ignore) continue;
//
//							// Проверка типов операндов
//							IT::IDDATATYPE rhsType = getDataType(rhsLex, idtable);
//							if (rhsType != IT::UNDEF)
//							{
//								// Если справа функция - проверяем её возвращаемый тип (он записан в iddatatype функции)
//								if (rhsType != targetType)
//								{
//									// Если target=INT, а rhs=CHAR (или наоборот) - допустимо? 
//									// Если строгая типизация - ошибка.
//									ERROR_THROW_IN(701, rhsLex.sn, -1);
//								}
//							}
//						}
//					}
//					break;
//				}
//
//				// --- Return ---
//				case LT_RETURN:
//				{
//					// Проверяем совместимость возвращаемого значения с типом функции
//					int nextIdx = i + 1;
//					// Пропуск скобки, если return (x);
//					if (nextIdx < lextable.size && lextable.table[nextIdx].lexema == LT_LEFTHESIS) nextIdx++;
//
//					if (nextIdx < lextable.size)
//					{
//						LT::Entry& retVal = lextable.table[nextIdx];
//						IT::IDDATATYPE retType = getDataType(retVal, idtable);
//
//						if (retType != IT::UNDEF && currentFuncType != IT::UNDEF)
//						{
//							if (retType != currentFuncType)
//							{
//								ERROR_THROW_IN(703, lexE.sn, -1);
//							}
//						}
//					}
//					break;
//				}
//
//				// --- Вызов функции ---
//				case LT_ID:
//				{
//					// Проверяем паттерн: ID (
//					if (i + 1 < lextable.size && lextable.table[i + 1].lexema == LT_LEFTHESIS)
//					{
//						// Это вызов функции?
//						if (idE && idE->idtype == IT::IDTYPE::F)
//						{
//							int paramIdx = 0;
//							int k = i + 2; // Первый параметр
//
//							while (k < lextable.size)
//							{
//								LT::Entry& pLex = lextable.table[k];
//								if (pLex.lexema == LT_RIGHTHESIS) break;
//
//								// Если параметр - ID или Literal
//								if (pLex.lexema == LT_ID || pLex.lexema == LT_LITERAL)
//
///*
//
//*/
//								{
//									paramIdx++;
//									if (paramIdx > idE->params.count)
//										ERROR_THROW_IN(705, lexE.sn, -1); // Много параметров
//
//									IT::IDDATATYPE paramType = getDataType(pLex, idtable);
//									IT::IDDATATYPE expectedType = idE->params.types[paramIdx - 1];
//
//									if (paramType != expectedType)
//										ERROR_THROW_IN(704, pLex.sn, pLex.tn); // Неверный тип
//								}
//								k++;
//							}
//
//							if (paramIdx < idE->params.count)
//								ERROR_THROW_IN(705, lexE.sn, -1); // Мало параметров
//						}
//					}
//					break;
//				}
//
//				} // switch
//			} // for
//		}
//	