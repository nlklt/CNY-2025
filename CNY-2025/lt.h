#pragma once

#define LT_MAXSIZE	    4096
#define LT_NULLIDX	    0xfffff

#define LT_MAIN		    'm'
#define LT_TYPE 		't'
#define LT_FOR	        'c'
#define LT_FUNCTION     'f'
#define LT_RETURN		'r'

#define LT_ID			'i'
#define LT_LITERAL		'l'

#define LT_EQUAL		'='
#define LT_OP_UNARY     'u'
#define LT_OP_BINARY    'v'

#define LT_RANGE        '.'
#define LT_COMMA    	','
#define LT_SEMICOLON	';'
#define LT_LEFTBRACE	'{'
#define LT_RIGHTBRACE	'}'
#define LT_LEFTHESIS	'('
#define LT_RIGHTHESIS   ')'

//таблица лексем
namespace LT {
    enum SIGNATURE
    {
        sign,

        t_int,
        t_char,
        t_string,

        variable,
        function,
        call,
        parameter,

        print,
        date,
        time,

        increment,
        dicrement,
        inversion,

        increment_post,
        dicrement_post,
        inversion_post,

        pref_increment,
        pref_dicrement,
        pref_inversion,

        plus,
        minus,
        division,
        multiplication
    };

    struct Entry
    {
        char lexema = '-';              // один символ - лексема
        char sign   = SIGNATURE::sign;  // значение лексемы (какой именннот тип(t_int/t_str), операция(+, -, /, *), и тд.)
        int  sn     = -1;               // номер строки
        int  tn     = -1;               // номер токена
        int  idxIT  = -1;               // индекс в таблице идентификаторов

        Entry() = default;

        Entry(char lexema_, char sign_, int sn_, int tn_, int idxIT_)
            : lexema(lexema_), sign(sign_), sn(sn_), tn(tn_), idxIT(idxIT_) { }
    };

    struct LexTable
    {
        int maxsize;
        int size;
        Entry* table;
    };

    LexTable Create(int size);
    void Add(LexTable& lextable, Entry entry);
    Entry GetEntry(LexTable& lextable, int n);
    void Delete(LexTable& lextable);
}
