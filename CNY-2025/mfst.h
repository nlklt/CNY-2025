#include "error.h"
#include "lt.h"
#include "grb.h"
#include "ast.h"

#include <stack>
#include <iostream>
#include <iomanip>

#define NS(n)	GRB::Rule::Chain::N(n)
#define TS(n)	GRB::Rule::Chain::T(n)
#define ISNS(n)	GRB::Rule::Chain::isN(n)

#define MFST_DIAGN_NUMBER 3

/*СТЕК АВТОМАТА*/
class MFSTSTSTACK : public std::stack<short>
{
public:
	using std::stack<short>::c;	//наследование скрытого члена, смена области видимости
};

namespace MFST
{
	/*СОСТОЯНИЕ АВТОМАТА(для сохранения)*/
	struct MfstState
	{
		short lenta_position;	//позиция на ленте
		short nrule;			//номер текущего правила
		short nrulechain;		//номер текущей цепочки, в текущем правиле
		MFSTSTSTACK st;			//стек автомата

		MfstState();
		MfstState(short pposition, MFSTSTSTACK pst, short pnrulechain);
		MfstState(short pposition, MFSTSTSTACK pst, short pnrule, short pnrulechain);
	};

	/*СТЕК АВТОМАТА*/
	class MSTSTSTATE : public std::stack<MfstState>
	{
	public:
		using std::stack<MfstState>::c;	//наследование скрытого члена, смена области видимости
	};

	/*МАГАЗИННЫЙ АВТОМАТ*/
	struct Mfst
	{
		//Код возврата step
		enum RC_STEP {
			NS_OK,			//найдено правило и цепочка, записана в стек
			NS_NORULE,		//ошибка в грамматике - не найдено рпвило
			NS_NORULECHAIN,	//ошибка в исходном коде - не найденаподходящая цепочка
			NS_ERROR,		//неизвестный нетерминальный символ
			TS_OK,			//символ ленты == вершине стека
			TS_NOK,			//символ ленты != вершине стека
			LENTA_END,		//текущая позиция ленты >= lenta_size
			SURPRISE		//ошибка в step - неожиданный код возврата
		};

		/*ДИАГНОСТИКА*/
		struct MfstDiagnosis
		{
			short lenta_position;	//позиция на ленте
			RC_STEP rc_step;		//код завершения шага
			short nrule;			//номер текущего правила
			short nrule_chain;		//номер текущей цепочки, в текущем правиле

			MfstDiagnosis();
			MfstDiagnosis(short lenta_position, RC_STEP rc_step, short nrule, short pnrule_chain);
		} diagnosis[MFST_DIAGN_NUMBER];

		GRBALPHABET* lenta;		//перекодированная (TN/NS) лента (из LEX)
		short lenta_position;	//текущая позиция на ленте
		short nrule;			//номер текущего правила
		short nrulechain;		//номер текущей цепочки, в текущем правиле
		short lenta_size;		//размер ленты
		GRB::Greibach grebach;	//граматика Грейбах
		LT::LexTable* lex;		//результат работы лексического анализатора
		MFSTSTSTACK st;			//стек автомата
		MSTSTSTATE storestate;	//стек для сохранения состояний
		Ast::Tree tree;

		Mfst();
		Mfst(LT::LexTable& plex, GRB::Greibach pgrebach);

		//Сохранить состояние автомата
		bool savestate();
		//Востановить состояние автомата
		bool reststate();
		//Поместить цепочку правила в стек
		bool push_chain(GRB::Rule::Chain chain);
		//Выполнить шаг автомата
		RC_STEP step();
		//Запустить автомат
		bool start(IT::IdTable idtable);
		//Сохранение диагностики работы МП-автомата
		bool savediagnosis(RC_STEP pprc_step);
		// Вывод использованных правил
		void printrules();

		/*ВЫВОД*/
		struct Deducation
		{
			short size;			//кол-во шагов в выводе
			short* nrules;		//номера правил граматики
			short* nrulechains;	//номера цепочек правил граматики(rules)
			Deducation()
			{
				size = 0;
				nrules = 0;
				nrulechains = 0;
			};
		} deducation;

		//Сохранить дерево вывода
		bool savededucation();
		// Преобразует линейную историю разбора (storestate) в иерархическое дерево
		bool buildTree(IT::IdTable& idtable);
	};
}
