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

        //// MFST_TRACE_START
            MFST::Mfst mfst(lextable, GRB::getGreibach());

        bool ok = mfst.start();
        if (ok)
        {
            std::cout << "Синтаксический анализ прошёл успешно." << std::endl;
        }
        else
        {
            std::cout << "Синтаксический анализ обнаружил ошибки." << std::endl;
        }

        mfst.savededucation();						//сохранить вывести правила вывода

        //mfst.printrules();							//отладка: вывести правила вывода

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