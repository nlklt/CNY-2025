#include "generation.h"
#include <vector>
#include <stack>
#include <sstream>
#include <iostream>

using namespace std;

namespace GN
{
	void GenerationASM(std::ostream* stream, LT::LexTable& lextable, IT::IdTable& idtable)
	{
		ostream* file = stream;
		*file << BEGIN;
		*file << EXTERN;
		*file << STACK(4096);
		GenConstAndData(idtable, file);
		*file << CODE;
		GenCode(lextable, idtable, file);
		*file << END;
	}

	void GenConstAndData(IT::IdTable& idtable, ostream* file)
	{
		vector <string> result;
		vector <string> con;	con.push_back(CONST);
		vector <string> data;	data.push_back(DATA);

		for (int i = 0; i < idtable.size; i++)
		{
			string str = "\t" + idtable.table[i].fullName;

			if (idtable.table[i].idtype == IT::IDTYPE::L) {			// литерал
				switch (idtable.table[i].iddatatype) {
				case IT::IDDATATYPE::INT:  str += " sdword " + itoS(idtable.table[i].value.vint);  break;
				case IT::IDDATATYPE::STR:
					if (string(idtable.table[i].value.vstr) != "")  str += " byte \"" + string(idtable.table[i].value.vstr) + "\" , 0";
					else str += " byte 0, 0";
					break;
				case IT::IDDATATYPE::CHR:							// !!! сделать для char
					if (string(idtable.table[i].value.vstr) != "")  str += " byte \"" + string(idtable.table[i].value.vstr) + "\" , 0";
					else str += " byte 0, 0";
					break;
				}
				con.push_back(str);
			}
			else if (idtable.table[i].idtype == IT::IDTYPE::V) {	// переменная
				switch (idtable.table[i].iddatatype) {
				case IT::IDDATATYPE::INT: str += " sdword 0";  break;	//sdword 0 - 4 байта со знаком
				case IT::IDDATATYPE::STR: str += " dword ?";  break;	//dword  4 байта без знака
				case IT::IDDATATYPE::CHR: str += " dword ?";  break;	//dword  4 байта без знака
				}
				data.push_back(str);
			}
		}
		result.insert(result.end(), con.begin(), con.end());	//result.push_back("\n");
		result.insert(result.end(), data.begin(), data.end());	//result.push_back("\n");
		for (auto r : result) {
			*file << r << endl;
		}
	}

	void GenCode(LT::LexTable& lextable, IT::IdTable& idtable, ostream* file) {
		string str;
		string funcName;	// имя текущей функции

		funcName = "main";
		str = SEPSTR("MAIN")
			+ "main PROC\n";
		*file << str << endl;
		str = "";

		for (int i = 0; i < lextable.size; i++) {
			switch (LT_ENTRY(i).lexema) {
				case LT_ID: {
					if (IT_ENTRY(i).idtype == IT::IDTYPE::C) {	// вывод
						if (LT_ENTRY(i).sign == LT::SIGNATURE::print) {
							PN::polishNotation(i, lextable, idtable);
							IT::Entry e = IT_ENTRY(i);
							switch (e.iddatatype) {
							case IT::IDDATATYPE::INT:
									str += "push " + string(e.fullName) + "\ncall write_int";
								break;
							case IT::IDDATATYPE::STR:
								if (e.idtype == IT::IDTYPE::L) {
									str += "\npush offset " + string(e.fullName) + "\ncall write_str";
								}
								else {
									str += "push " + string(e.fullName) + "\ncall write_str";
								}
								break;
							case IT::IDDATATYPE::CHR:
								if (e.idtype == IT::IDTYPE::L) {
									str += "push offset " + string(e.fullName) + "\ncall write_str";
								}
								else {
									str += "push " + string(e.fullName) + "\ncall write_str";
								}
								break;
							}
						}
						else {									// вызов
							PN::polishNotation(i, lextable, idtable);
							str = GenCallFuncCode(lextable, idtable, i);
						}
					}
					break;
				}
				case LT_FUNCTION: {								// объявление функции
					funcName = IT_ENTRY(i + 2).fullName;
					str = GenFunctionCode(lextable, idtable, i);
					break;
				}
				case LT_RETURN: {								// return
					str = GenExitCode(lextable, idtable, i, funcName);
					break;
				}
				case LT_EQUAL: {								// присваивание
					PN::polishNotation(i + 1, lextable, idtable);
					str = GenEqualCode(lextable, idtable, i);
					break;
				}
			}
			if (!str.empty()) {
				*file << str << endl;
				str.clear();
			}
		}
	}

	string GenEqualCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i) {
		string str;
		IT::Entry e1 = IT_ENTRY(i - 1); // левый операнд
		i++;
		switch (e1.iddatatype) {
		case IT::IDDATATYPE::INT: {
			for (; LT_ENTRY(i).lexema != LT_SEMICOLON; i++) {
				switch (LT_ENTRY(i).lexema) {
				case LT_LITERAL:
				case LT_ID: {
					if (IT_ENTRY(i).idtype == IT::IDTYPE::C) {				// если в выражении вызов функции
						str = str + GenCallFuncCode(lextable, idtable, i);	// функция возвращает результат в eax
						str = str + "push eax\n";	// результат выражения в стек для дальнейшего вычисления выражения
						break;
					}
					else {
						str = str + "push " + IT_ENTRY(i).fullName + "\n";
					}
					break;
				}
				case LT_OP_BINARY:
					switch (LT_ENTRY(i).sign) {
					case LT::SIGNATURE::plus:
						str += "pop ebx\npop eax\nadd eax, ebx\npush eax\n"; break;
					case LT::SIGNATURE::minus:
						str += "pop ebx\npop eax\nsub eax, ebx\npush eax\n"; break;
					case LT::SIGNATURE::multiplication:
						str += "pop ebx\npop eax\nimul eax, ebx\npush eax\n"; break;
					case LT::SIGNATURE::division:
						str += "pop ebx\npop eax\ncdq\nidiv ebx\npush eax\n"; break;
					}
				}
			}
			str += "pop " + string(e1.fullName) + '\n';
			break;
		}
		case IT::IDDATATYPE::STR: {
			char lex = LT_ENTRY(i).lexema;
			IT::Entry e2 = IT_ENTRY(i);
			if (lex == LT_ID && (e2.idtype == IT::IDTYPE::P)) {	// вызов функции
				str += GenCallFuncCode(lextable, idtable, i);
				str += "mov " + string(e1.fullName) + ", eax";
			}
			else if (lex == LT_LITERAL) {	// литерал
				str = "mov " + string(e1.fullName) + ", offset " + string(e2.fullName);
			}
			else {							// переменная
				str += "mov ecx, " + string(e2.fullName) + "\nmov " + string(e1.fullName) + ", ecx";
			}
		}
		}
		return str;
	}

	string GenFunctionCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i) {
		string str = string(IT_ENTRY(i + 2).fullName) + string(" PROC,\t");
		//funcName = IT_ENTRY(i + 2).fullName;
		i += 5; // дальше параметры, сразу после открывающей скобки

		while (LT_ENTRY(i).lexema != LT_RIGHTHESIS) {	// пока параметры не кончатся
			if (LT_ENTRY(i).lexema == LT_ID) {			// параметр
				str += string(IT_ENTRY(i).fullName) + (IT_ENTRY(i).iddatatype == IT::IDDATATYPE::INT ? " : sdword, " : " : dword, ");
			}
			++i;
		}
		str += "\n";

		return str;
	}

	string GenExitCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i, string funcname) {
		string str = "; --- восстановить регистры --- \npop edx \npop ebx \n; -----------------------------\n";
		if (LT_ENTRY(i + 1).lexema != LT_SEMICOLON) {
			str += "mov eax, " + string(IT_ENTRY(i + 1).fullName) + "\n";
		}

		if (!funcname.empty() && funcname != "main") {
			str += "ret\n";
			str += funcname + " ENDP" + SEPSTREMP;
		}
		else { }
		return str;
	}

	string GenCallFuncCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i) {
		string str;
		stack <IT::Entry> temp;
		for (i++; LT_ENTRY(i).lexema != '@'; i++) {
			if (LT_ENTRY(i).lexema == LT_ID || LT_ENTRY(i).lexema == LT_LITERAL) {
				temp.push(IT_ENTRY(i));	// заполняем стек в прямом порядке	
			}
		}
		IT::Entry e = IT_ENTRY(++i);	// идентификатор вызываемой функции
		while (!temp.empty()) {			// раскручиваем стек
			if (temp.top().idtype == IT::IDTYPE::L && temp.top().iddatatype == IT::IDDATATYPE::STR ||
				temp.top().iddatatype == IT::IDDATATYPE::CHR) {	// !!!
				str += "push offset " + string(temp.top().fullName) + "\n";
			}
			else {
				str += "push " + string(temp.top().fullName) + "\n";
			}
			temp.pop();
		}

		str += "call " + string(e.fullName) + '\n';
		i++;

		return str;
	}

	string itoS(int x) //чтобы избежать дублирования меток
	{
		stringstream r;  r << x;  return r.str();
	}
}


