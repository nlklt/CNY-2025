#include "error.h"
#include "parm.h"
#include "in.h"
#include "lt.h"
#include "it.h"


#include "lexer.h"
#include "semantic.h"
#include "log.h"

#include "grb.h"	
#include "mfst.h"

#include "polis.h"

#include <iostream>
#include <locale>
#include <tchar.h>

int wmain(int argc, wchar_t* argv[]) {
    setlocale(LC_ALL, "rus");

    Log::LOG log = Log::INITLOG;

    LT::LexTable lextable = LT::Create(LT_MAXSIZE);
    IT::IdTable idtable = IT::Create(TI_MAXSIZE);

    try {
        Parm::PARM parm = Parm::getparm(argc, argv);

        log = Log::getlog(parm.log);

        Log::WriteLog(log);
        Log::WriteParm(log, parm);

        In::IN in = In::getin(parm.in, parm.out);

        Log::WriteIn(log, in);

        Lexer::lexicalAnalysis(in, lextable, idtable);

        Log::WriteLex(log, lextable, &idtable);
        Log::WriteIdTable(log, idtable);
        Log::WriteLexTable(log, lextable, &idtable);

        MFST::Mfst mfst(lextable, GRB::getGreibach());

        bool ok = mfst.start(idtable);
        if (ok)
        {
            std::cout << "Синтаксический анализ прошёл успешно." << std::endl;

            mfst.buildTree(idtable);

            if (mfst.tree.toDot("ast.dot"))
            {
                std::cout << "Дерево успешно сохранено в файл ast.dot." << std::endl;
                std::cout << "Для визуализации используйте команду Graphviz." << std::endl;
            }
            else
            {
                std::cerr << "Ошибка записи в файл ast.dot." << std::endl;
            }

            std::cout << "\n=== Абстрактное Синтаксическое Дерево (AST) ===" << std::endl;
            mfst.tree.print(mfst.tree.root, 0);
            std::cout << "===============================================" << std::endl;
        }
        else
        {
            std::cout << "Синтаксический анализ обнаружил ошибки." << std::endl;
        }

        mfst.printrules();

        LT::Entry eof;
        eof.lexema = EOF;
        LT::Add(lextable, eof);

        // Semantic::semanticAnalysis(lextable, idtable);
        // std::cout << "\nСемантический анализ завершен без ошибок";

        // Polis::polishNotation(0, lextable, idtable);
        // sintaxAnaliz.printRules(log.stream);

        Log::WriteLine(log, (char*)"Успешное завершение.", "");
        Log::Close(log);

        LT::Delete(lextable);
        IT::Delete(idtable);
    }

    catch (Error::ERROR e) {
        std::cout << "[ОШИБКА] в строке: " << e.inext.line << " на позиции: " << e.inext.col << ": " << e.message << "\n";
        Log::Close(log);
        return -1;
    }

    Log::Close(log);
    return 0;
}

	//int main() {
	//	for (size_t i = 0; i < 16; i++)
	//	{
	//		for (size_t j = 0; j < 16; j++)
	//		{
	//			char c = i * 16 + j;
	//			unsigned char uc = static_cast<unsigned char>(c);
	//			if (uc >= 'A' && uc <= 'Z' || uc >= 'a' && uc <= 'z' || uc == '_' || uc >= '0' && uc <= '9' ||
	//				uc >= (unsigned char)'А' && uc <= (unsigned char)'Я' || uc >= (unsigned char)'а' && uc <= (unsigned char)'я' || uc == (unsigned char)'Ё' || uc == (unsigned char)'ё' ||
	//				uc == '=' || uc == '+' || uc == '-' || uc == '*' || uc == '/' || uc == '~' ||
	//				uc == '.' || uc == ',' || uc == ';' || uc == ' ' || uc == '\t'|| 
	//				uc == '(' || uc == ')' || uc == '{' || uc == '}' || uc == '\'' || uc == '"'
	//				)
	//				std::cout << "IN::T, ";
	//			else if (uc == '\n' || uc == '#')
	//				std::cout << "IN::IN_SEPORATOR, ";
	//			else if (uc == '\r')
	//				std::cout << "IN::I, ";
	//			else
	//				std::cout << "IN::F, ";
	//		}
	//		std::cout << "\\\n";
	//	}
	//}