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
        std::cout << Colors::GREEN << "Параметры командной строки получены успешно." << Colors::RESET << std::endl;

        log = Log::getlog(parm.log);

        Log::WriteLog(log);
        Log::WriteParm(log, parm);

        In::IN in = In::getin(parm.in, parm.out);
        std::cout << Colors::GREEN << "Проверка на недопустимые символы прошла успешно." << Colors::RESET << std::endl;

        Log::WriteIn(log, in);

        Lexer::lexicalAnalysis(in, lextable, idtable);
        std::cout << Colors::GREEN << "Лексический анализ прошёл успешно." << Colors::RESET << std::endl;

        Log::WriteLex(log, lextable, &idtable);
        Log::WriteLexTable(log, lextable, &idtable);

        MFST::Mfst mfst(lextable, GRB::getGreibach());
        bool ok = mfst.start(idtable);
        if (ok)
        {
            std::cout << Colors::GREEN << "Синтаксический анализ прошёл успешно." << Colors::RESET << std::endl;

            mfst.buildTree(idtable);

            if (mfst.tree.toDot("pst.dot")) {
                std::cout << Colors::GREEN << "Дерево разбора успешно сохранено в файл pst.dot." << Colors::RESET << std::endl;
            }
            else {
                std::cerr << Colors::RED << "Ошибка записи в файл pst.dot." << Colors::RESET << std::endl;
            }

            mfst.tree.print(mfst.tree.root, 0, log.stream);
        }
        else
        {
            std::cout << Colors::RED << "Синтаксический анализ обнаружил ошибки." << Colors::RESET << std::endl;
        }

        SM::semanticAnalysis(lextable, idtable);

        std::cout << Colors::GREEN << "Семантический анализ прошёл успешно." << Colors::RESET << std::endl;;

        Log::WriteLine(log, (char*)"Успешное завершение.", "");
        Log::Close(log);

        std::ofstream asmFile(parm.out);
        if (!asmFile.is_open())
        {
            std::cout << Colors::RED << "Ошибка: Не удалось открыть файл " << parm.out << Colors::RESET << std::endl;
        }

        GN::GenerationASM(&asmFile, lextable, idtable);

        LT::Delete(lextable);
        IT::Delete(idtable);

        asmFile.close();

        std::cout << Colors::GREEN << "Успешное завершение." << Colors::RESET << std::endl;;

        Log::WriteLine(log, (char*)"Ассемблер успешно сгенерирован.\n", "");
        Log::WriteLine(log, (char*)"---------- Результат генерации --------\n", "");
        WriteLine(log, (wchar_t*)L"Файл с исходным кодом на языке ассемблер находится по пути: ", (wchar_t*)parm.out, L"");
    }

    catch (Error::ERROR e) {
        std::cout << Colors::RED << "Error: В строке " << e.inext.line << " на позиции " << e.inext.col << ": " << e.message << "\n";
        Log::Close(log);
        return -1;
    }

    Log::Close(log);
    return 0;
}
