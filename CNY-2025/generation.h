#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <stdexcept>

#include "lt.h"
#include "it.h"
#include "polis.h"

#define IT_ENTRY(x)		idtable.table[lextable.table[x].idxIT]
#define LT_ENTRY(x)		lextable.table[x]

#define SEPSTREMP  ";------------------------------\n"
#define SEPSTR(x)  (std::string("\n;----------- ") + std::string(x) + " ------------\n")

#define BEGIN			  ".386\n"\
					   << ".model flat, stdcall		\n"\
					   << "includelib kernel32.lib	\n"\
					   << "includelib libucrt.lib	\n"\
					   << "includelib Library.lib	\n\n"\
\
					   << "ExitProcess PROTO: dword	\n"

#define EXTERN			  "write_int PROTO C :sword\n"\
					   << "write_str PROTO C :ptr byte\n"\
					   << "get_time PROTO C	\n"\
					   << "get_date PROTO C\n"

#define STACK(value)	".stack 4096 \n"

#define CONST			"\n.const"

#define DATA			"\n.data"

#define CODE			"\n.code"

#define END				"push 0\ncall ExitProcess\nmain ENDP\nend main"



namespace GN
{
	void GenerationASM(std::ostream* stream, LT::LexTable& lextable, IT::IdTable& idtable);
	void GenConstAndData(IT::IdTable& idtable, std::ostream* file);
	void GenCode(LT::LexTable& lextable, IT::IdTable& idtable, std::ostream* file);

	std::string GenEqualCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i);
	std::string GenFunctionCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i);
	std::string GenExitCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i, std::string funcname);
	std::string GenCallFuncCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i);

	std::string itoS(int x);
}


// case LT_IF: // условие
// {
// 	branchingnNum++;
// 	str = GenBranchingCode(lextable, idtable, i, branchingnNum);
// 	break;
// }
// case LT_LEFTBRACE: {
// 	open++;
// 	break;
// }
// case LT_RIGHTBRACE:	// переход на метку в конце кондишна
// {
// 	open--;
// 	if (LT_ENTRY(i + 1).lexema == LT_ELSE)
// 	{
// 		kol.pop();
// 		kol.push(A(open, branchingnNum, IfEnum::thenOrElse));
// 		str += "jmp next" + itoS(kol.top().branchingnNum) + '\n';
// 	}
// 	else
// 		if (!kol.empty())
// 			if (kol.top().openRightbrace == open)
// 			{
// 				if (kol.top().ifEnum == IfEnum::repeat)
// 				{
// 					str += "jmp cyclenext" + itoS(kol.top().branchingnNum) + '\n';
// 					str += "cycle" + itoS(kol.top().branchingnNum) + ":\n";
// 					kol.pop();
// 				}
// 				else
// 				{
// 					str += "next" + itoS(kol.top().branchingnNum) + ":\n";
// 					kol.pop();
// 				}
// 			}
// 	break;
// }
//		case LT_THEN: // условие верно (метка)
//		{
//			kol.push(A(open, branchingnNum, IfEnum::thenOrElse));
//			str += "true" + itoS(branchingnNum) + ":";
//			break;
//		}
//		case LT_ELSE: // условие неверно(метка)
//		{
//			str += "false" + itoS(branchingnNum) + ":";
//			break;
//		}
//		case LT_REPEAT: // цикл с условием (метка)
//		{
//			branchingnNum++;
//			kol.push(A(open, branchingnNum, IfEnum::repeat));
//			str += "cyclenext" + itoS(kol.top().branchingnNum) + ":\n";
//			str += GenBranchingCode(lextable, idtable, i, kol.top().branchingnNum);

//			break;
//		}

/*	string GenBranchingCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i, int branchingnNum)
	{
		string str;
		IT::Entry lft = IT_ENTRY(i + 2); // левый операнд
		IT::Entry rgt = IT_ENTRY(i + 4); // правый операнд
		bool f = false, t = false;
		string fstr, tstr;

		str += "mov edx, " + lft.fullName + "\ncmp edx, " + rgt.fullName + "\n";
		// switch (LT_ENTRY(i + 3).lexema)
		// {//метки переходов в процессе сравнения
		// case LT_MORE:  tstr = "jg";  fstr = "jl";  break; //JG - если первый операнд больше второго          jl - если первый операнд меньше второго
		// case LT_LESS:   tstr = "jl";  fstr = "jg";  break;
		// case LT_COMPARE: tstr = "jz";  fstr = "jnz";  break; //jz - переход, если равно        jnz - переход, если не равно
		// case LT_NOTEQUALS:   tstr = "jnz";  fstr = "jz";  break;
		// }

		// if (LT_ENTRY(i).lexema != LT_REPEAT)
		// {
		// 	for (int j = i + 6; LT_ENTRY(j - 2).lexema != LT_RIGHTBRACE; j++) // пропустили условие
		// 	{
		// 		if (LT_ENTRY(j).lexema == LT_THEN)
		// 			t = true;
		// 		if (LT_ENTRY(j).lexema == LT_ELSE)
		// 			f = true;
		// 	}
		// 	if (t) str += "\n" + tstr + " true" + itoS(branchingnNum);
		// 	if (f) str += "\n" + fstr + " false" + itoS(branchingnNum);
		// 	if (!t || !f)  str = str + "\njmp next" + itoS(branchingnNum);
		// }
		// else
		// {
		// 	str += fstr + " cycle" + itoS(branchingnNum);
		// }

		return str;
	}*/