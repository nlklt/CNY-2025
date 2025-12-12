#include "generation.h"
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

        SM::semanticAnalysis(lextable, idtable);

        std::cout << "Успешное завершение.";

        Log::WriteLex(log, lextable, &idtable);
        Log::WriteIdTable(log, idtable);
        Log::WriteLexTable(log, lextable, &idtable);

        Log::WriteLine(log, (char*)"Успешное завершение.", "");
        Log::Close(log);

        std::ofstream asmFile(parm.out);
        if (!asmFile.is_open())
        {
            std::cout << "Ошибка: Не удалось открыть файл " << parm.out << std::endl;
        }

        GN::Generate(lextable, idtable, &asmFile);

        LT::Delete(lextable);
        IT::Delete(idtable);

        asmFile.close();

        Log::WriteLine(log, (char*)"Ассемблер успешно сгенерирован.\n", "");
        Log::WriteLine(log, (char*)"---------- Результат генерации --------\n", "");
        WriteLine(log, (wchar_t*)L"Файл с исходным кодом на языке ассемблер находится по пути: ", (wchar_t*)parm.out, L"");
    }

    catch (Error::ERROR e) {
        std::cout << "[ОШИБКА] в строке: " << e.inext.line << " на позиции: " << e.inext.col << ": " << e.message << "\n";
        Log::Close(log);
        return -1;
    }

    Log::Close(log);
    return 0;
}

//#include <iostream>
//int main() {
//	int i = 1 + 1 * 3;
//	char a = 'i';
//	int b = i * a - i + a;
//	std::cout << b;
//
//
//	return 0;
//}
