// generation.cpp — упрощённый генератор ASM (MASM x86)
// В этой версии extrn-декларации формируются динамически после
// анализа таблицы идентификаторов, чтобы избежать конфликта имён
// (например, если пользователь определил функцию get_char).
// Требует lt.h, it.h (как у вас).

#include "lt.h"
#include "it.h"

#include <string>
#include <vector>
#include <stack>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cctype>
#include <sstream>

using namespace std;

namespace GN {

    static string SanitizeName(const string& s) {
        if (s.empty()) return "anon";
        string out;
        bool first = true;
        for (unsigned char uc : s) {
            if (isalnum(uc) || uc == '_') {
                if (first && isdigit(uc)) out.push_back('_');
                out.push_back((char)uc);
            }
            else out.push_back('_');
            first = false;
        }
        return out;
    }

    static string MakeVarAsmName(const IT::Entry& e, int idx) {
        string base = SanitizeName(e.id);
        string scope = SanitizeName(e.scope.empty() ? "g" : e.scope);
        return base + "_" + scope + "_" + to_string(idx);
    }

    static string GetUniqueLiteralName(const IT::Entry& entry, int index) {
        if (entry.iddatatype == IT::INT) return "Lnum_" + to_string(index);
        if (entry.iddatatype == IT::STR) return "Lstr_" + to_string(index);
        if (entry.iddatatype == IT::CHR) return "Lchar_" + to_string(index);
        return "Llit_" + to_string(index);
    }

    // ---------------- HEADER ----------------
    // Убираем явные extrn для всех runtime-функций из HEADER —
    // их будем генерировать динамически после анализа idtable,
    // чтобы избежать конфликта с пользовательскими определениями.
    static const char* HEADER =
        ".586\n"
        ".model flat, stdcall\n"
        ".stack 4096\n"
        "includelib kernel32.lib\n"
        "includelib Library.lib\n\n";

    // forward declarations
    void GenConstAndData(LT::LexTable& lextable, IT::IdTable& idtable, ostream* file,
        unordered_map<int, string>& asmNames,
        unordered_map<int, string>& literalNames,
        bool& hasMain);

    void GenCode(LT::LexTable& lextable, IT::IdTable& idtable, ostream* file,
        unordered_map<int, string>& asmNames,
        unordered_map<int, string>& literalNames);

    bool GenStatementsRange(int start, int end,
        LT::LexTable& lextable, IT::IdTable& idtable, ostream* file,
        unordered_map<int, string>& asmNames,
        unordered_map<int, string>& literalNames);

    string GenExpression(LT::LexTable& lextable, IT::IdTable& idtable, int& i,
        unordered_map<int, string>& asmNames,
        unordered_map<int, string>& literalNames,
        IT::IDDATATYPE* outType = nullptr);

    void Generate(LT::LexTable& lextable, IT::IdTable& idtable, std::ostream* stream) {
        unordered_map<int, string> asmNames;
        unordered_map<int, string> literalNames;
        bool hasMain = false;

        *stream << HEADER;
        GenConstAndData(lextable, idtable, stream, asmNames, literalNames, hasMain);

        // Список runtime-функций, которые могут быть внешними
        vector<string> runtimeNames = { "ExitProcess", "write_int", "write_str", "get_time", "get_date", "get_char" };

        // Собираем имена функций, определённых пользователем, чтобы не объявлять их как extrn
        unordered_set<string> declaredFuncs;
        for (int i = 0; i < idtable.size; ++i) {
            if (idtable.table[i].idtype == IT::F || idtable.table[i].idtype == IT::C) {
                auto it = asmNames.find(i);
                if (it != asmNames.end()) declaredFuncs.insert(it->second);
            }
        }

        // Выводим extrn только для тех runtime-функций, которые НЕ определены пользователем
        *stream << "\n";
        for (const string& rn : runtimeNames) {
            if (declaredFuncs.count(rn) == 0) {
                *stream << "extrn " << rn << ":proc\n";
            }
        }
        *stream << "\n";

        *stream << "\n.code\n";
        GenCode(lextable, idtable, stream, asmNames, literalNames);

        if (hasMain) *stream << "end main\n";
        else *stream << "end\n";
    }

    // ---------- .const / .data ----------
    void GenConstAndData(LT::LexTable& /*lextable*/, IT::IdTable& idtable, ostream* file,
        unordered_map<int, string>& asmNames,
        unordered_map<int, string>& literalNames,
        bool& hasMain) {

        *file << ".const\n";
        *file << "\tnewline byte 13, 10, 0\n";
        *file << "\ttrue_str byte 'true', 0\n";
        *file << "\tfalse_str byte 'false', 0\n\n";

        int litIndex = 0;
        for (int i = 0; i < idtable.size; ++i) {
            const IT::Entry& e = idtable.table[i];
            if (e.idtype == IT::L || (e.idtype == IT::UNKNOWN && (e.iddatatype == IT::INT || e.iddatatype == IT::STR || e.iddatatype == IT::CHR))) {
                string lname = GetUniqueLiteralName(e, litIndex++);
                literalNames[i] = lname;
                if (e.iddatatype == IT::INT) {
                    *file << "\t" << lname << " sdword " << e.value.vint << "\n";
                }
                else if (e.iddatatype == IT::STR) {
                    string s = e.value.vstr;
                    string esc;
                    for (char c : s) { if (c == '\'') esc += "''"; else esc += c; }
                    if (esc.empty()) *file << "\t" << lname << " byte 0\n";
                    else *file << "\t" << lname << " byte '" << esc << "', 0\n";
                }
                else if (e.iddatatype == IT::CHR) {
                    *file << "\t" << lname << " dword " << static_cast<int>(e.value.vchr) << "\n";
                }
            }
        }

        *file << "\n.data\n";
        *file << "\tswitch_res sdword 0\n";

        // prepare asm names
        for (int i = 0; i < idtable.size; ++i) {
            const IT::Entry& e = idtable.table[i];
            if (e.idtype == IT::F || e.idtype == IT::C) {
                asmNames[i] = SanitizeName(e.id);
                if (SanitizeName(e.id) == "main") hasMain = true;
            }
            else {
                asmNames[i] = MakeVarAsmName(e, i);
            }
        }

        // declare variables/params/locals (to avoid undefined symbols)
        unordered_set<string> declared;
        for (int i = 0; i < idtable.size; ++i) {
            const IT::Entry& e = idtable.table[i];
            if (e.idtype == IT::V || e.idtype == IT::P) {
                string nm = asmNames[i];
                if (declared.count(nm)) continue;
                declared.insert(nm);

                if (e.iddatatype == IT::STR) {
                    *file << "\t" << nm << " dword ?\n";
                }
                else if (e.iddatatype == IT::INT) {
                    // Инициализируем глобальные/локальные числовые переменные нулём по умолчанию,
                    // чтобы избежать записи мусорных значений в объектный файл.
                    *file << "\t" << nm << " sdword 0\n";
                }
                else if (e.iddatatype == IT::CHR) {
                    *file << "\t" << nm << " dword 0\n";
                }
                else {
                    *file << "\t" << nm << " dword 0\n";
                }
            }
        }

        *file << "\n";
    }

    static int FindMatchingRbrace(LT::LexTable& lextable, int pos) {
        int depth = 0;
        for (int k = pos; k < lextable.size; ++k) {
            if (lextable.table[k].lexema == LT_LEFTBRACE) ++depth;
            else if (lextable.table[k].lexema == LT_RIGHTBRACE) {
                --depth;
                if (depth == 0) return k;
            }
        }
        return -1;
    }

    // ---------- Statements ---------- (returns true if emitted return)
    bool GenStatementsRange(int start, int end,
        LT::LexTable& lextable, IT::IdTable& idtable, ostream* file,
        unordered_map<int, string>& asmNames,
        unordered_map<int, string>& literalNames) {

        int k = start;
        while (k <= end) {
            char lex = lextable.table[k].lexema;
            if (lex == LT_SEMICOLON) { ++k; continue; }

            if (lex == LT_RETURN) {
                ++k;
                string expr = GenExpression(lextable, idtable, k, asmNames, literalNames);
                *file << expr;
                *file << "\tpop eax\n\tret\n";
                return true;
            }

            if (lex == LT_ID && (k + 1 <= end) && lextable.table[k + 1].lexema == LT_EQUAL) {
                int leftIdx = lextable.table[k].idxIT;
                k += 2;
                string expr = GenExpression(lextable, idtable, k, asmNames, literalNames);
                *file << expr;
                *file << "\tpop eax\n\tmov " << asmNames[leftIdx] << ", eax\n";
                if (k <= end && lextable.table[k].lexema == LT_SEMICOLON) ++k;
                continue;
            }

            if (lex == LT_FOR) {
                int p = k + 1;
                if (p < lextable.size && lextable.table[p].lexema == LT_TYPE) ++p;
                if (p < lextable.size && lextable.table[p].lexema == LT_ID) {
                    int idIdx = lextable.table[p].idxIT;
                    ++p;
                    if (p < lextable.size && lextable.table[p].lexema == LT_EQUAL) ++p;
                    string startExpr = GenExpression(lextable, idtable, p, asmNames, literalNames);
                    while (p < lextable.size && lextable.table[p].lexema != LT_RANGE) ++p;
                    if (p < lextable.size && lextable.table[p].lexema == LT_RANGE) ++p;
                    string endExpr = GenExpression(lextable, idtable, p, asmNames, literalNames);
                    while (p < lextable.size && lextable.table[p].lexema != LT_LEFTBRACE) ++p;
                    int braceOpen = p;
                    int braceClose = FindMatchingRbrace(lextable, braceOpen);
                    if (braceOpen >= 0 && braceClose >= 0) {
                        static int loopCounter = 0;
                        int loopId = loopCounter++;
                        string labelStart = "LOOP_START_" + to_string(loopId);
                        string labelEnd = "LOOP_END_" + to_string(loopId);

                        *file << startExpr;
                        *file << "\tpop eax\n\tmov " << asmNames[idIdx] << ", eax\n";
                        *file << labelStart << ":\n";
                        *file << "\tmov eax, " << asmNames[idIdx] << "\n";
                        *file << endExpr;
                        *file << "\tpop ebx\n";
                        *file << "\tcmp eax, ebx\n";
                        *file << "\tjg " << labelEnd << "\n";

                        bool bodyReturned = GenStatementsRange(braceOpen + 1, braceClose - 1, lextable, idtable, file, asmNames, literalNames);
                        if (bodyReturned) {
                            *file << labelEnd << ":\n";
                            k = braceClose + 1;
                            continue;
                        }
                        *file << "\tinc " << asmNames[idIdx] << "\n";
                        *file << "\tjmp " << labelStart << "\n";
                        *file << labelEnd << ":\n";
                    }
                    k = (braceClose > 0 ? braceClose + 1 : p);
                    continue;
                }
            }

            if (lex == LT_LITERAL || lex == LT_ID) {
                string code = GenExpression(lextable, idtable, k, asmNames, literalNames);
                *file << code;
                if (k < (int)lextable.size && lextable.table[k].lexema == LT_SEMICOLON) {
                    *file << "\tpop eax\n";
                    ++k;
                }
                continue;
            }

            ++k;
        }
        return false;
    }

    // ---------- Function block (plain label, no PROC/ENDP) ----------
    static void GenerateFunctionBlock(int& i, LT::LexTable& lextable, IT::IdTable& idtable, ostream* file,
        unordered_map<int, string>& asmNames,
        unordered_map<int, string>& literalNames) {

        bool isMain = (lextable.table[i].lexema == LT_MAIN);

        int funcIdIdx = -1;
        for (int k = i; k < lextable.size; ++k) {
            if (lextable.table[k].lexema == LT_ID) {
                int idx = lextable.table[k].idxIT;
                if (idx >= 0 && idx < idtable.size) {
                    if (idtable.table[idx].idtype == IT::F || idtable.table[idx].idtype == IT::C) { funcIdIdx = idx; break; }
                    if (lextable.table[k + 1].lexema == LT_LEFTHESIS || lextable.table[k + 1].lexema == LT_LEFTBRACE) { funcIdIdx = idx; break; }
                }
            }
            if (lextable.table[k].lexema == LT_LEFTBRACE) break;
        }

        string funcAsmName;
        if (isMain) funcAsmName = "main";
        else if (funcIdIdx >= 0) funcAsmName = asmNames[funcIdIdx];
        else funcAsmName = "func_unknown_" + to_string(i);

        // emit label instead of PROC to avoid MASM PROC/PROTO conflicts
        *file << funcAsmName << ":\n";

        // find '{'
        int bracePos = -1;
        for (int k = i; k < lextable.size; ++k) if (lextable.table[k].lexema == LT_LEFTBRACE) { bracePos = k; break; }
        if (bracePos == -1) { *file << "\tret\n"; return; }

        int endBrace = FindMatchingRbrace(lextable, bracePos);
        if (endBrace == -1) endBrace = bracePos;

        bool bodyReturned = GenStatementsRange(bracePos + 1, endBrace - 1, lextable, idtable, file, asmNames, literalNames);

        if (!bodyReturned) {
            if (isMain) {
                *file << "\tpush 0\n\tcall ExitProcess\n";
            }
            else {
                *file << "\tret\n";
            }
        }

        i = endBrace;
    }

    // ---------- Top-level ----------
    void GenCode(LT::LexTable& lextable, IT::IdTable& idtable, ostream* file,
        unordered_map<int, string>& asmNames,
        unordered_map<int, string>& literalNames) {

        for (int i = 0; i < lextable.size; ++i) {
            char lex = lextable.table[i].lexema;
            if (lex == LT_FUNCTION || lex == LT_MAIN) {
                GenerateFunctionBlock(i, lextable, idtable, file, asmNames, literalNames);
            }
        }
    }

    // ---------- Expressions ----------
    string GenExpression(LT::LexTable& lextable, IT::IdTable& idtable, int& i,
        unordered_map<int, string>& asmNames,
        unordered_map<int, string>& literalNames,
        IT::IDDATATYPE* outType) {

        string code;
        IT::IDDATATYPE currentType = IT::INT;

        while (i < lextable.size) {
            char lex = lextable.table[i].lexema;
            if (lex == LT_SEMICOLON || lex == LT_COMMA || lex == LT_RIGHTHESIS || lex == LT_RIGHTBRACE || lex == LT_LEFTBRACE) break;

            if (lex == LT_LITERAL || lex == LT_ID) {
                int idx = lextable.table[i].idxIT;
                if (idx >= 0 && idx < idtable.size) {
                    const IT::Entry& e = idtable.table[idx];
                    if (e.idtype == IT::L || (e.idtype == IT::UNKNOWN && (e.iddatatype == IT::INT || e.iddatatype == IT::STR || e.iddatatype == IT::CHR))) {
                        string lname = literalNames.at(idx);
                        if (e.iddatatype == IT::STR) { code += "\tpush offset " + lname + "\n"; currentType = IT::STR; }
                        else { code += "\tpush " + lname + "\n"; currentType = IT::INT; }
                        ++i; continue;
                    }
                    else if (e.idtype == IT::V || e.idtype == IT::P) {
                        string an = asmNames.at(idx);
                        code += "\tpush " + an + "\n";
                        currentType = e.iddatatype;
                        ++i; continue;
                    }
                    else if (e.idtype == IT::C || e.idtype == IT::F) {
                        int p = i + 1;
                        bool hasParen = (p < lextable.size && lextable.table[p].lexema == LT_LEFTHESIS);
                        if (hasParen) {
                            p++;
                            vector<string> args;
                            vector<IT::IDDATATYPE> argTypes;
                            if (p < lextable.size && lextable.table[p].lexema != LT_RIGHTHESIS) {
                                while (p < lextable.size) {
                                    IT::IDDATATYPE at = IT::INT;
                                    string argCode = GenExpression(lextable, idtable, p, asmNames, literalNames, &at);
                                    args.push_back(argCode);
                                    argTypes.push_back(at);
                                    if (p < lextable.size && lextable.table[p].lexema == LT_COMMA) { ++p; continue; }
                                    if (p < lextable.size && lextable.table[p].lexema == LT_RIGHTHESIS) break;
                                    break;
                                }
                            }
                            for (int k = (int)args.size() - 1; k >= 0; --k) code += args[k];
                            int totalArgBytes = static_cast<int>(args.size()) * 4;
                            string funcSan = SanitizeName(e.id);
                            string funcAsm = asmNames.count(idx) ? asmNames[idx] : funcSan;
                            if (funcSan == "print") {
                                if (!argTypes.empty() && argTypes[0] == IT::STR) {
                                    code += "\tcall write_str\n";
                                    code += "\tadd esp, " + to_string(totalArgBytes) + "\n";
                                    code += "\tpush offset newline\n\tcall write_str\n\tadd esp, 4\n";
                                    code += "\tpush 0\n";
                                    currentType = IT::INT;
                                }
                                else {
                                    code += "\tcall write_int\n";
                                    code += "\tadd esp, " + to_string(totalArgBytes) + "\n";
                                    code += "\tpush offset newline\n\tcall write_str\n\tadd esp, 4\n";
                                    code += "\tpush 0\n";
                                    currentType = IT::INT;
                                }
                            }
                            else {
                                code += "\tcall " + funcAsm + "\n";
                                if (totalArgBytes > 0) code += "\tadd esp, " + to_string(totalArgBytes) + "\n";
                                code += "\tpush eax\n";
                                currentType = e.iddatatype;
                            }
                            i = p; ++i; continue;
                        }
                        else {
                            string funcSan = SanitizeName(e.id);
                            string funcAsm = asmNames.count(idx) ? asmNames[idx] : funcSan;
                            if (funcSan == "print") {
                                if (currentType == IT::STR) code += "\tcall write_str\n";
                                else code += "\tcall write_int\n";
                                code += "\tadd esp, 4\n";
                                code += "\tpush offset newline\n\tcall write_str\n\tadd esp, 4\n";
                                code += "\tpush 0\n";
                                currentType = IT::INT;
                            }
                            else {
                                code += "\tcall " + funcAsm + "\n";
                                code += "\tpush eax\n";
                                currentType = e.iddatatype;
                            }
                            ++i; continue;
                        }
                    }
                }
                else { ++i; continue; }
            }

            if (lex == LT_OP_UNARY) {
                LT::SIGNATURE sig = (LT::SIGNATURE)lextable.table[i].sign;
                if (sig == LT::SIGNATURE::increment) { code += "\tpop eax\n\tinc eax\n\tpush eax\n"; }
                else if (sig == LT::SIGNATURE::inversion) { code += "\tpop eax\n\tnot eax\n\tpush eax\n"; }
                else if (sig == LT::SIGNATURE::minus) { code += "\tpop eax\n\tneg eax\n\tpush eax\n"; }
                else { code += "\t; unknown unary\n"; }
                ++i; continue;
            }

            if (lextable.table[i].lexema == LT_OP_BINARY) {
                LT::SIGNATURE sig = (LT::SIGNATURE)lextable.table[i].sign;
                code += "\tpop ebx\n\tpop eax\n";
                static int __div_lab = 0;
                if (sig == LT::SIGNATURE::plus) code += "\tadd eax, ebx\n";
                else if (sig == LT::SIGNATURE::minus) code += "\tsub eax, ebx\n";
                else if (sig == LT::SIGNATURE::multiplication) code += "\timul ebx\n";
                else if (sig == LT::SIGNATURE::division) {
                    int id = __div_lab++;
                    string ok = "DIV_OK_" + to_string(id);
                    string end = "DIV_END_" + to_string(id);
                    code += "\ttest ebx, ebx\n\tjnz " + ok + "\n";
                    code += "\tpush 0\n\tjmp " + end + "\n";
                    code += ok + ":\n\tcdq\n\tidiv ebx\n";
                    code += end + ":\n";
                }
                code += "\tpush eax\n";
                currentType = IT::INT;
                ++i; continue;
            }

            ++i;
        }

        if (outType) *outType = currentType;
        return code;
    }

} // namespace GN

// Global wrapper
extern "C" void Generate(LT::LexTable& lextable, IT::IdTable& idtable, std::ostream* stream) {
    GN::Generate(lextable, idtable, stream);
}
