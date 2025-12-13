#include "generation.h"
#include "error.h"
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
				case IT::IDDATATYPE::CHR:							// !!! сделать дл€ char
					if (string(idtable.table[i].value.vstr) != "")  str += " byte \"" + string(idtable.table[i].value.vstr) + "\" , 0";
					else str += " byte 0, 0";
					break;
				}
				con.push_back(str);
			}
			else if (idtable.table[i].idtype == IT::IDTYPE::V) {	// переменна€
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
		string funcName;	// им€ текущей функции

		for (int i = 0; i < lextable.size; i++) {
			switch (LT_ENTRY(i).lexema) {
				case LT_MAIN: {
					funcName = "main";
					str = SEPSTR("MAIN")
						+ "main PROC\n";
					break;
				}
				case LT_ID: {
					if (IT_ENTRY(i).idtype == IT::IDTYPE::C) {	// вывод
						if (LT_ENTRY(i).sign == LT::SIGNATURE::print) {
							PN::polishNotation(i, lextable, idtable);
							IT::Entry e = IT_ENTRY(i);
							switch (e.iddatatype) {
							case IT::IDDATATYPE::INT:
									str += "push " + string(e.fullName) + "\ncall write_int\nadd esp, 4";
								break;
							case IT::IDDATATYPE::STR:
								if (e.idtype == IT::IDTYPE::L) {
									str += "\npush offset " + string(e.fullName) + "\ncall write_str\nadd esp, 4";
								}
								else {
									str += "push " + string(e.fullName) + "\ncall write_str\nadd esp, 4";
								}
								break;
							case IT::IDDATATYPE::CHR:
								if (e.idtype == IT::IDTYPE::L) {
									str += "push offset " + string(e.fullName) + "\ncall write_str\nadd esp, 4";
								}
								else {
									str += "push " + string(e.fullName) + "\ncall write_str\nadd esp, 4";
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
				case LT_FUNCTION: {								// объ€вление функции
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
					str = str + "push " + IT_ENTRY(i).fullName + "\n";
					break;
				}
				case '@': {				// вызов функции
					str += "call " + string(IT_ENTRY(++i).fullName) + '\n';
					str = str + "push eax\n";	// результат выражени€ в стек дл€ дальнейшего вычислени€ выражени€
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
			else {							// переменна€
				str += "mov ecx, " + string(e2.fullName) + "\nmov " + string(e1.fullName) + ", ecx";
			}
		}
		}
		return str;
	}

	string GenFunctionCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i) {
		std::vector<std::string> args;
		string funcName = IT_ENTRY(i + 2).fullName;
		string arg = "";
		string str = "\n" + funcName + " PROC, ";
		i += 5; // ѕереход к параметрам

		bool first = true;
		while (i < lextable.size && LT_ENTRY(i).lexema != LT_RIGHTHESIS) {
			if (LT_ENTRY(i).lexema == LT_ID) {
				arg = (string(IT_ENTRY(i).fullName) + (IT_ENTRY(i).iddatatype == IT::IDDATATYPE::INT ? " : sdword" : " : dword"));
				if (first) { first = false; }
				else { arg += ", "; }
				args.push_back(arg);
			}
			++i;
		}

		for (int k = (int)args.size() - 1; k >= 0; --k) {
			str += args[k];
		}
		
		str += "\n\tpush ebp\n\n";
		str += "\tpush ebx\n\tpush esi\n\tpush edi\n";
		return str;
	}


	string GenCallFuncCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i) {
		string str;
		vector<IT::Entry> args;

		// i указывает на исходную позицию (обычно позици€ до operand list), переходим к следующему токену
		++i;
		// собираем аргументы до маркера '@' (у PN должен быть такой маркер)
		while (i < lextable.size && lextable.table[i].lexema != '@') {
			char lx = lextable.table[i].lexema;
			if (lx == LT_ID || lx == LT_LITERAL) {
				// убедимс€ что idxIT валиден
				if (lextable.table[i].idxIT == IT_NULLIDX) {
					// некорректна€ ссылка на id Ч бросаем ошибку
					ERROR_THROW(110 /*код ошибки*/, lextable.table[i].line, 0);
				}
				args.push_back(idtable.table[lextable.table[i].idxIT]);
			}
			++i;
		}

		// если '@' не найдено Ч ошибка
		if (i >= lextable.size) {
			ERROR_THROW(111 /* '@' not found */, /*line*/0, /*pos*/0);
		}

		// следующий токен Ч идентификатор функции
		++i;
		if (i >= lextable.size || lextable.table[i].lexema != LT_ID || lextable.table[i].idxIT == IT_NULLIDX) {
			ERROR_THROW(112 /* invalid function identifier after @ */);
		}
		IT::Entry funcEntry = idtable.table[lextable.table[i].idxIT];

		// генерируем push'и: правый аргумент первым (right-to-left)
		for (int k = (int)args.size() - 1; k >= 0; --k) {
			IT::Entry& a = args[k];
			if (a.idtype == IT::IDTYPE::L && (a.iddatatype == IT::IDDATATYPE::STR || a.iddatatype == IT::IDDATATYPE::CHR)) {
				str += "\tpush offset " + string(a.fullName) + "\n";
			}
			else {
				str += "\tpush " + string(a.fullName) + "\n";
			}
		}

		// вызов
		str += "\tcall " + string(funcEntry.fullName) + "\n";

		++i;

		if (args.size() > 0) {
			str += "\tadd esp, " + to_string(args.size() * 4) + "\n";
		}

		return str;
	}


	static int FindFunctionIndex(IT::IdTable& idtable, const std::string& fullName) {
		for (int k = 0; k < idtable.size; ++k) {
			if (idtable.table[k].fullName == fullName &&
				(idtable.table[k].idtype == IT::IDTYPE::F || idtable.table[k].idtype == IT::IDTYPE::C)) {
				return k;
			}
		}
		return IT_NULLIDX;
	}

	string GenExitCode(LT::LexTable& lextable, IT::IdTable& idtable, int& i, string funcname) {
		string str;

		// восстановление сохранЄнных регистров (симметрично prlog)
		str += "\t; --- восстановить регистры --- \n";
		str += "\tpop edi\n";  // ≈сли сохран€ли EDI
		str += "\tpop esi\n";
		str += "\tpop ebx\n";
		str += "\t; -----------------------------\n";

		str += "\tmov esp, ebp\n"; // ESP теперь указывает на сохраненный EBP
		str += "\tpop ebp\n";      // EBP восстановлен, ESP указывает на адрес возврата

		// если есть возвращаемое выражение: mov eax, <expr>
		if (i + 1 < lextable.size && LT_ENTRY(i + 1).lexema != LT_SEMICOLON) {
			// ожидание, что lextable содержит идентификатор/литерал/или переменную
			str += "\tmov eax, " + string(IT_ENTRY(i + 1).fullName) + "\n";
		}

		// если это не main Ч вернуть с очисткой параметров (stdcall)
		if (!funcname.empty() && funcname != "main") {
			// найдем функцию в idtable, чтобы узнать кол-во параметров (если хранитс€)
			int idx = FindFunctionIndex(idtable, funcname);
			int bytesToClean = 0;
			if (idx != IT_NULLIDX) {
				int cnt = idtable.table[idx].params.count; // предполагаем, что count хранитс€
				bytesToClean = cnt * 4;
			}
			// ret с очисткой стека (если bytesToClean == 0 Ч это просто ret 0 Ч эквивалент ret)
			if (bytesToClean > 0) {
				str += "\tret " + itoS(bytesToClean) + "\n";
			}
			else {
				str += "\tret\n";
			}
			str += funcname + " ENDP" + SEPSTREMP;
		}
		else {
			// дл€ main Ч просто ret (считайте, что ExitProcess вызываетс€)
			str += "\t; main end (no ret/ENDP here)\n";
		}

		return str;
	}

	string itoS(int x) //чтобы избежать дублировани€ меток
	{
		stringstream r;  r << x;  return r.str();
	}
}


