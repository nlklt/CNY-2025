#pragma once

#include <vector>
#include <string>

#define TI_MAXSIZE      4096
#define IT_NULLIDX      0xfffff

#define IT_ID_MAXSIZE   32

#define IT_INT_DEFAULT  0
#define IT_CHAR_DEFAULT 'c'
#define IT_CHAR_MAXSIZE  1
#define IT_STR_DEFAULT  "s"
#define IT_STR_MAXSIZE  255

//таблица идентификаторов
namespace IT {
    enum IDTYPE { V, F, P, C, L, UNKNOWN };
    enum IDDATATYPE { INT, CHR, STR, NONE, UNDEF };

    struct Entry {
        int         idxfirstLT = -1;
        std::string id         = "-";
        std::string scope      = "-";
        IDTYPE      idtype     = UNKNOWN;
        IDDATATYPE  iddatatype = UNDEF;
        struct Value
        {
            int  vint = IT_INT_DEFAULT;
            char vstr[IT_STR_MAXSIZE] = IT_STR_DEFAULT;
            char vchr = IT_CHAR_DEFAULT;
        } value;
        struct Params
        {
            int count = -1;
            std::vector<IDDATATYPE> types;
        } params;
        std::string fullName = "-";
        bool isDefined = false;

        Entry() = default;

        Entry(int idxfirstLT_, const std::string& id_, const std::string& scope_,
            IDTYPE idtype_, IDDATATYPE iddatatype_, const std::string& fullName_)
            : idxfirstLT(idxfirstLT_), id(id_), scope(scope_),
            idtype(idtype_), iddatatype(iddatatype_), fullName(fullName_) { }

        Entry(int idxfirstLT_, const std::string& id_, IDTYPE idtype_, IDDATATYPE iddatatype_)
            : idxfirstLT(idxfirstLT_), id(id_), idtype(idtype_), iddatatype(iddatatype_) { }
    };

    struct IdTable {
        int maxsize;
        int size;
        Entry* table;
    };

    IdTable Create(int size);
    void Add(IdTable& idtable, Entry& entry);
    Entry GetEntry(IdTable& idtable, int n);
    int IsId(IdTable& idtable, std::string fullName);
    //const char* GetDataTypeString(int iddatatype);
    //const char* GetIdTypeString(int idtype);
    void Delete(IdTable& idtable);
}