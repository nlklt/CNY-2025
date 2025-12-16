#include "grb.h"

#include <vector>
#include <string> 
#include <cstdarg>
#include "mfst.h"

#define GRB_ERROR_SERIES 600

namespace GRB
{
#define NS(n) Rule::Chain::N(n)
#define TS(n) Rule::Chain::T(n)

	Greibach greibach(NS('S'), TS('$'), {	// стартовый символ, дно стека

			Rule(NS('S'), GRB_ERROR_SERIES + 0, {	// Неверная тсруктура программы

					{ TS('f'), TS('t'), TS('i'), TS('('),  NS('F'), TS(')'), TS('{'), NS('N'), TS('}'), NS('S')},
					{ TS('f'), TS('t'), TS('i'), TS('('), TS(')'), TS('{'), NS('N'), TS('}'), NS('S')},
					{ TS('t'), TS('i'), TS(';'), NS('S') },
					{ TS('i'), TS('='), NS('E'), TS(';'), NS('S') },
					{ TS('t'), TS('i'), TS('='), NS('E'), TS(';'), NS('S') },
					{ TS('t'), TS('m'), TS('('), TS(')'), TS('{'), NS('N'), TS('}'), NS('S') },
					{ TS('$') }
			}),
			Rule(NS('N'), GRB_ERROR_SERIES + 1, {	//Ошибочный оператор

					{ TS('t'), TS('i'), TS(';'), NS('N') },
					{ TS('i'), TS('='), NS('E'), TS(';'), NS('N') },
					{ TS('t'), TS('i'), TS('='), NS('E'), TS(';'), NS('N') },

					{ TS('i'), TS('('), NS('W'), TS(')'), TS(';'), NS('N')},						// вызов функции
					{ TS('i'), TS('('), TS(')'), TS(';'), NS('N')},						// вызов функции
					
					{ TS('}') },
					{ TS('c'), TS('t'), TS('i'), TS('='), NS('E'), TS('.'), NS('E'), TS('{'), NS('N'), NS('N')},

					{ TS('i'), TS('u'), TS(';'), NS('N') },
					{ TS('i'), TS('u'), NS('M'), TS(';'), NS('N') },
					{ TS('u'), TS('i'), TS(';'), NS('N') },
					{ TS('u'), TS('i'), NS('M'), TS(';'), NS('N') },
					
					{ TS('r'), NS('E'), TS(';') },
					{ TS('r'), NS('E'), TS(';'), NS('N') }
			}),
			Rule(NS('E'), GRB_ERROR_SERIES + 2, {	//Ошибка в выражении

					{ TS('i') },							// идентификатор
					{ TS('l') },							// литерал
					{ TS('i'), NS('M') },					// идентификатор с продолжением(действий)
					{ TS('l'), NS('M') },					// литерал с операциями
															
					{ TS('u'), TS('i') },					// унарная операция с идентификатором
					{ TS('u'), TS('l') },					// унарная операция с литералом
					{ TS('u'), TS('i'), NS('M')},			// унарная операция с идентификатором с продолжением(действий)

					{ TS('('), NS('E'), TS(')') },			// выражение в скобках
					{ TS('('), NS('E'), TS(')'), NS('M') },	// выражение в скобках с прододжением(действий)

					{ TS('i'), TS('('), NS('W'), TS(')') },				// вызов функции
					{ TS('i'), TS('('), TS(')') },				// вызов функции
					{ TS('i'), TS('('), TS(')'), NS('M') },	// вызов функции с продолжением(действий)
					{ TS('i'), TS('('), NS('W'), TS(')'), NS('M') },	// вызов функции с продолжением(действий)
			}),
			Rule(NS('M'), GRB_ERROR_SERIES + 3,	{	//Ошибка в действиях

					{ TS('u') },
					{ TS('u'), NS('M')},
					{ TS('v'), NS('E') },
					{ TS('v'), NS('E'), NS('M') }
			}),
			Rule(NS('F'), GRB_ERROR_SERIES + 4,	{	//Ошибка в объявлении

					{ TS('t'), TS('i'), TS(','), NS('F') }, // несколько параметров
					{ TS('t'), TS('i') }                    // одиночный параметр
			}),
			Rule(NS('W'), GRB_ERROR_SERIES + 5, {	//Ошибка в параметрах

					{ TS('i'), TS(','), NS('W') },
					{ TS('l'), TS(','), NS('W') },
					{ TS('i'), TS('('),NS('W'), TS(')')  },
					{ TS('i'), TS('('), TS(')')  },
					{ TS('i') },
					{ TS('l') }
			})
		});

	Greibach getGreibach()
	{
		return greibach;
	};

	short Greibach::getRule(GRBALPHABET pnn, Rule& prule)
	{
		for (int i = 0; i < rules.size(); i++)
		{
			if (rules[i].nn == pnn)
			{
				prule = rules[i];
				return i;
			}
		}
		return -1;
	};

	Rule Greibach::getRule(short n)
	{
		if (n >= 0 && n < rules.size())
		{
			return rules[n];
		}
		return Rule();
	}

	char* Rule::getCRule(char* b, short nchain)
	{
		if (nchain >= 0 && nchain < chains.size())
		{
			char bchain[200];
			b[0] = Chain::alphabet_to_char(nn);
			b[1] = '-';
			b[2] = '>';
			b[3] = 0x00;
			chains[nchain].getCChain(bchain);
			strcat_s(b, 205, bchain);
			return b;
		}
		b[0] = 0x00;
		return b;
	};

	short Rule::getNextChain(GRBALPHABET t, Rule::Chain& pchain, short j)
	{
		for (short i = j; i < chains.size(); i++)
		{
			if (!chains[i].nt.empty() && chains[i].nt[0] == t)
			{
				pchain = chains[i];
				return i;
			}
		}
		return -1;
	};

	char* Rule::Chain::getCChain(char* b)
	{
		for (int i = 0; i < nt.size(); i++)
		{
			b[i] = Chain::alphabet_to_char(nt[i]);
		}
		b[nt.size()] = 0x00;
		return b;
	}
}