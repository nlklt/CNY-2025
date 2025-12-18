#pragma once

#include "in.h"
#include "lt.h"
#include "it.h"
#include "parm.h"
#include "error.h"

#include <fstream>
#include <cstdarg>

// Логирование
namespace Log
{
    // ПРОТОКОЛ (имя файла и поток для записи)
    struct LOG
    {
        wchar_t logfile[PARM_MAX_SIZE]; // имя файла протокола
        std::ofstream* stream;          // выходной поток протокола
    };

    static const LOG INITLOG = { L"", NULL };    // начальная инициализации LOG

    // Создание/закрытие
    LOG getlog(wchar_t logfile[]);               // сформировать структуру LOG (открыть файл)
    void Close(LOG log);                         // закрыть протокол

    // Простые записи
    void WriteLine(LOG log, char* c, ...);       // вывести в протокол конкатенацию строк (C-style, заканчивается пустой строкой "")
    void WriteLine(LOG log, const wchar_t* c, ...);   // аналог для wide-строк

    // Общий заголовок/параметры/вход/ошибка
    void WriteLog(LOG log);                       // вывести в протокол заголовок (дата создания)
    void WriteParm(LOG log, Parm::PARM parm);     // вывести в протокол информацию о входных параметрах
    void WriteIn(LOG log, In::IN in);             // вывести в протокол информацию о входном потоке
    void WriteError(LOG log, Error::ERROR error); // вывести в протокол информацию об ошибке

    // Таблицы (WriteLexTable может принимать дополнительную таблицу идентификаторов для расширенного вывода)
    void WriteLexTable(std::ostream& out, const LT::LexTable& lt, const IT::IdTable* it);
    void WriteIdTable(std::ostream& out, const IT::IdTable& it);
    void WriteLex(LOG log, LT::LexTable& lt, IT::IdTable* it);
}
