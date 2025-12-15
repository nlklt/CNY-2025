#include "generation.h"
#include "error.h"
#include <vector>
#include <stack>
#include <string>

using namespace std;

namespace GN {
    // Структура для хранения информации о текущем цикле в стеке
    struct LoopInfo {
        string startLabel;   // Метка начала (проверка условия)
        string endLabel;     // Метка конца (выход)
        string iteratorName; // Имя переменной-счетчика (i)
    };

    string GenPrintCall(IT::Entry& e) {
        string out;
        if (e.iddatatype == IT::IDDATATYPE::STR) {
            out += "\tcall write_str\n\tadd esp, 4\n";
        }
        else {
            out += "\tcall write_int\n\tadd esp, 4\n";
        }
        return out;
    }

    void GenerationASM(std::ostream* stream, LT::LexTable& lextable, IT::IdTable& idtable) {
        *stream << BEGIN_ASM << EXTERN_ASM << STACK_ASM(4096);
        GenConstAndData(idtable, stream);
        *stream << CODE_ASM;
        GenCode(lextable, idtable, stream);
        *stream << END_ASM;
    }

    void GenConstAndData(IT::IdTable& idtable, ostream* file) {
        *file << "\n.const\n";
        for (int i = 0; i < idtable.size; i++) {
            if (idtable.table[i].idtype == IT::IDTYPE::L) {
                *file << "\t" << idtable.table[i].fullName;
                if (idtable.table[i].iddatatype == IT::IDDATATYPE::INT)
                    *file << " dd " << idtable.table[i].value.vint << "\n";
                else {
                    string s = idtable.table[i].value.vstr;
                    *file << " db \"" << s << "\", 0\n";
                }
            }
        }

        *file << "\n.data\n";
        for (int i = 0; i < idtable.size; i++) {
            if (idtable.table[i].idtype == IT::IDTYPE::V || idtable.table[i].idtype == IT::IDTYPE::P) {
                *file << "\t" << idtable.table[i].fullName << " dd 0\n";
            }
        }
    }

    void GenCode(LT::LexTable& lextable, IT::IdTable& idtable, ostream* file) {
        string currFuncName = "";

        stack<LoopInfo> loopStack;
        int labelCounter = 1;

        for (int i = 0; i < lextable.size; i++) {
            char lex = lextable.table[i].lexema;

            if (lex == LT_ID && (lextable.table[i].sign == LT::SIGNATURE::call || lextable.table[i].sign == LT::SIGNATURE::print ||
                lextable.table[i].sign == LT::SIGNATURE::time || lextable.table[i].sign == LT::SIGNATURE::date)) {
                PN::polishNotation(i, lextable, idtable);
                *file << ProcessPolishString(lextable, idtable, i);
            }
            else if (lex == 'c') {
                string iterName = idtable.table[lextable.table[i + 2].idxIT].fullName;

                string startVal;
                if (lextable.table[i + 4].lexema == LT_LITERAL)
                    startVal = to_string(idtable.table[lextable.table[i + 4].idxIT].value.vint);
                else
                    startVal = idtable.table[lextable.table[i + 4].idxIT].fullName;

                string endVal;
                if (lextable.table[i + 6].lexema == LT_LITERAL)
                    endVal = to_string(idtable.table[lextable.table[i + 6].idxIT].value.vint);
                else
                    endVal = idtable.table[lextable.table[i + 6].idxIT].fullName;

                string lblStart = "CYCLE_START_" + to_string(labelCounter);
                string lblEnd = "CYCLE_END_" + to_string(labelCounter);
                labelCounter++;

                loopStack.push({ lblStart, lblEnd, iterName });

                *file << "\n; --- Cycle Init ---\n";
                *file << "\tmov eax, " << startVal << "\n";
                *file << "\tmov " << iterName << ", eax\n";

                *file << lblStart << ":\n";
                *file << "\tmov eax, " << iterName << "\n";

                if (lextable.table[i + 6].lexema == LT_LITERAL) {
                    *file << "\tcmp eax, " << endVal << "\n";
                }
                else {
                    *file << "\tmov ebx, " << endVal << "\n";
                    *file << "\tcmp eax, ebx\n";
                }

                *file << "\tjge " << lblEnd << "\n";

                i += 7;
            }
            else if (lex == '}') {
                if (!loopStack.empty()) {
                    LoopInfo info = loopStack.top();
                    loopStack.pop();

                    *file << "\n; --- Cycle Step ---\n";
                    *file << "\tinc " << info.iteratorName << "\n"; // i++
                    *file << "\tjmp " << info.startLabel << "\n";   // На начало
                    *file << info.endLabel << ":\n";                // Метка выхода
                }
                else {}
            }

            else if (lex == LT_FUNCTION || lex == LT_MAIN) {
                std::vector<IT::Entry> parms;
                if (lex == LT_FUNCTION) {
                    currFuncName = idtable.table[lextable.table[i + 2].idxIT].fullName;
                    *file << "\n" << currFuncName << " PROC\n";
                    int j = i + 4;
                    while (lextable.table[j].lexema != LT_RIGHTHESIS) {
                        if (lextable.table[j].lexema == LT_ID) {
                            parms.push_back(idtable.table[lextable.table[j].idxIT]);
                        }
                        j++;
                    }
                }
                else {
                    currFuncName = "main";
                    *file << "\nmain PROC\n";
                }

                // пролог
                *file << "\tpush ebp\n\tmov ebp, esp\n";

                // Передача параметров через регистры (код из предыдущего ответа)
                if (currFuncName != "main") {
                    for (int p = 0; p < parms.size(); p++) {
                        string regName;
                        if (p == 0) regName = "eax";
                        else if (p == 1) regName = "ebx";
                        else if (p == 2) regName = "ecx";
                        else regName = "eax";

                        *file << "\tmov " << parms[p].fullName << ", " << regName << "\n";
                    }
                }
                *file << "\tpush ebx\n\tpush esi\n\tpush edi\n";
            }
            else if (lex == LT_EQUAL) {
                string target = idtable.table[lextable.table[i - 1].idxIT].fullName;
                ++i;
                PN::polishNotation(i, lextable, idtable);
                *file << ProcessPolishString(lextable, idtable, i);
                *file << "\tpop eax\n\tmov " << target << ", eax\n";
            }
            else if (lex == LT_RETURN) {
                i++;
                PN::polishNotation(i, lextable, idtable);
                *file << ProcessPolishString(lextable, idtable, i);
                *file << "\tpop eax\n";

                // эпилог
                *file << "\tpop edi\n\tpop esi\n\tpop ebx\n";
                *file << "\tmov esp, ebp\n\tpop ebp\n";

                if (currFuncName == "main") {
                    *file << "\tpush eax\n\tcall dword ptr [_imp__ExitProcess@4]\n";
                }
                else {
                    *file << "\tret\n";
                }
                *file << currFuncName << " ENDP\n";
            }
        }
    }

    string ProcessPolishString(LT::LexTable& lextable, IT::IdTable& idtable, int& i) {
        string out;
        for (; i < lextable.size; i++) {
            char lex = lextable.table[i].lexema;
            if (lex == LT_SEMICOLON) return out;
            if (lex == '#') continue;

            switch (lex) {
            case LT_LITERAL:
            case LT_ID: {
                IT::Entry e = idtable.table[lextable.table[i].idxIT];
                if (lextable.table[i].sign == LT::SIGNATURE::parameter) { break; }
                if (e.iddatatype == IT::IDDATATYPE::STR) {
                    if (LT_ENTRY(i).lexema == LT_LITERAL) {
                        out += "\tpush offset " + string(IT_ENTRY(i).fullName) + "\n";
                    }
                    else {
                        out += "\tpush " + string(IT_ENTRY(i).fullName) + "\n";
                    }
                }
                else {
                    out += "\tpush " + string(e.fullName) + "\n";
                }
                break;
            }
            case '@': {
                if (LT_ENTRY(i + 1).sign == LT::SIGNATURE::print) {
                    IT::Entry arg = idtable.table[lextable.table[i - 1].idxIT];
                    out += GenPrintCall(arg);
                    i += 1;
                    break;
                }
                std::vector<IT::Entry> args;
                int p = 1;
                while (i - p >= 0 && LT_ENTRY(i - p).sign == LT::SIGNATURE::parameter) {
                    args.push_back(idtable.table[lextable.table[i - p].idxIT]);
                    p++;
                }
                int argCount = args.size();
                for (int k = 0; k < argCount; k++) {
                    IT::Entry arg = args[argCount - 1 - k];
                    string val;
                    if (arg.iddatatype == IT::IDDATATYPE::STR && arg.idtype == IT::IDTYPE::L)
                        val = "offset " + string(arg.fullName);
                    else
                        val = string(arg.fullName);

                    if (k == 0) out += "\tmov eax, " + val + "\n";
                    else if (k == 1) out += "\tmov ebx, " + val + "\n";
                    else if (k == 2) out += "\tmov ecx, " + val + "\n";
                }

                IT::Entry f = idtable.table[lextable.table[++i].idxIT];
                out += "\tcall " + string(f.fullName) + "\n";
                out += "\tpush eax\n";
                break;
            }
            case LT_OP_BINARY: {
                out += "\tpop ebx\n\tpop eax\n";
                switch (lextable.table[i].sign) {
                case LT::SIGNATURE::plus:           out += "\tadd eax, ebx\n"; break;
                case LT::SIGNATURE::minus:          out += "\tsub eax, ebx\n"; break;
                case LT::SIGNATURE::multiplication: out += "\timul eax, ebx\n"; break;
                case LT::SIGNATURE::division:       out += "\tcdq\n\tidiv ebx\n"; break;
                }
                out += "\tpush eax\n";
                break;
            }
            case 'u': { // Обработка унарных операций
                // Для инкремента/декремента нам нужен адрес переменной, которая шла ПЕРЕД операцией
                // В ОПН это будет выглядеть так: i 'u'
                IT::Entry& var = idtable.table[lextable.table[i - 1].idxIT];

                switch (lextable.table[i].sign) {
                case LT::SIGNATURE::increment_post:
                    // x++ : Значение x уже в стеке. Просто инкрементируем переменную в памяти.
                    out += "\tinc " + string(var.fullName) + "\n";
                    break;

                case LT::SIGNATURE::pref_increment:
                    // ++x : Значение еще не в стеке (или там старое). 
                    // Правильнее: инкрементируем в памяти, потом пушим актуальное.
                    out += "\tinc " + string(var.fullName) + "\n";
                    out += "\tpush " + string(var.fullName) + "\n";
                    break;

                case LT::SIGNATURE::pref_inversion:
                    // ~x : Это просто операция над значением в стеке
                    out += "\tpop eax\n";
                    out += "\tnot eax\n";
                    out += "\tpush eax\n";
                    break;

                case LT::SIGNATURE::dicrement_post:
                    out += "\tdec " + string(var.fullName) + "\n";
                    break;

                case LT::SIGNATURE::pref_dicrement:
                    out += "\tdec " + string(var.fullName) + "\n";
                    out += "\tpush " + string(var.fullName) + "\n";
                    break;
                }
                break;
            }
            }
        }
        return out;
    }
}