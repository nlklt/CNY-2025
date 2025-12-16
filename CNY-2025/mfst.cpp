#include "mfst.h"
#include "it.h"

int FST_TRACE_n = -1;
char rbuf[205], sbuf[205], lbuf[1024];	//для печати

namespace MFST
{
	/*Конструкторы*/
	MfstState::MfstState() { lenta_position = -1; nrule = -1; nrulechain = -1; }

	MfstState::MfstState(short pposition, MFSTSTSTACK pst, short pnrulechain)
		: lenta_position(pposition), st(pst), nrulechain(pnrulechain) { }

	MfstState::MfstState(short pposition, MFSTSTSTACK pst, short pnrule, short pnrulechain)
		: lenta_position(pposition), st(pst), nrule(pnrule), nrulechain(pnrulechain) { }

	Mfst::MfstDiagnosis::MfstDiagnosis() { lenta_position = -1; rc_step = SURPRISE; nrule = -1; nrule_chain = -1; }

	Mfst::MfstDiagnosis::MfstDiagnosis(short plenta_position, RC_STEP prc_step, short pnrule, short prule_chain)
		: lenta_position(plenta_position), rc_step(prc_step), nrule(pnrule), nrule_chain(prule_chain) { }

	Mfst::Mfst() { lenta_size = lenta_position = 0; lenta = nullptr; lex = nullptr; nrule = -1; nrulechain = -1;
		// st, storestate, grebach вызываются конструкторами по умолчанию
	}

	Mfst::Mfst(LT::LexTable& plex, GRB::Greibach pgrebach) {
		lex = &plex;
		grebach = pgrebach;
		lenta_size = lex->size;
		lenta = new short[lenta_size];
		for (int k = 0; k < lenta_size; k++) {
			lenta[k] = TS(lex->table[k].lexema);
		}
		lenta_position = 0;
		st.push(grebach.stbottomT);
		st.push(grebach.startN);
		nrulechain = -1;
	}

	Mfst::RC_STEP Mfst::step() {
		RC_STEP rc = SURPRISE;
		if (lenta_position < lenta_size) {
			//Начинается с нетерминала?
			if (ISNS(st.top())) {
				GRB::Rule rule;
				if ((nrule = grebach.getRule(st.top(), rule)) >= 0) {
					GRB::Rule::Chain chain;
					if ((nrulechain = rule.getNextChain(lenta[lenta_position], chain, nrulechain + 1)) >= 0) {
						savestate();
						st.pop();
						push_chain(chain);
						rc = NS_OK;
					}
					else {
						savediagnosis(NS_NORULECHAIN);
						rc = reststate() ? NS_NORULECHAIN : NS_NORULE;
					};
				}
				else {
					rc = NS_ERROR;
				}
			}
			else if ((st.top() == lenta[lenta_position])) {
				lenta_position++;
				st.pop();
				nrulechain = -1;
				rc = TS_OK;
			}
			else {
				rc = reststate() ? TS_NOK : NS_NORULECHAIN;
			};
		}
		else {
			rc = LENTA_END;
		};
		return rc;
	};

	bool Mfst::push_chain(GRB::Rule::Chain chain) {
		for (int k = chain.nt.size() - 1; k >= 0; k--) {
			st.push(chain.nt[k]);
		}
		return true;
	};

	char* Mfst::getDiagnosis(short n, char* buf)
	{
		char* rc = 0;
		int errid = 0;
		int lpos = -1;
		if (n < MFST_DIAGN_NUMBER && (lpos = diagnosis[n].lenta_position) >= 0)
		{
			int errid = grebach.getRule(diagnosis[n].nrule).iderror;
			Error::ERROR err = Error::geterror(errid);
			sprintf_s(buf, MFST_DIAGN_MAXSIZE, "%d: строка %d, %s", err.id, lex->table[lpos].sn, err.message);
			rc = buf;
		};
		return rc;
	};

	bool Mfst::savestate() {
		storestate.push(MfstState(lenta_position, st, nrule, nrulechain));
		return true;
	};

	bool Mfst::reststate() {
		if (storestate.size() > 0) {
			MfstState state = storestate.top();
			lenta_position = state.lenta_position;
			st = state.st;
			nrule = state.nrule;
			nrulechain = state.nrulechain;
			storestate.pop();
			return true;
		};
		return false;
	};

	bool Mfst::savediagnosis(RC_STEP prc_step) {
		short k = 0;
		while (k < MFST_DIAGN_NUMBER && lenta_position <= diagnosis[k].lenta_position) {
			k++;
		}
		if (k < MFST_DIAGN_NUMBER) {
			diagnosis[k] = MfstDiagnosis(lenta_position, prc_step, nrule, nrulechain);
			for (short j = k = 1; j < MFST_DIAGN_NUMBER; j++) {
				diagnosis[j].lenta_position = -1;
			}
			return true;
		};
		return false;
	};

	bool Mfst::start(IT::IdTable idtable) {
		bool rc = false;
		RC_STEP rc_step = step();
		char buf[MFST_DIAGN_MAXSIZE];
		rc_step = step();
		while (rc_step == NS_OK || rc_step == NS_NORULECHAIN || rc_step == TS_OK || rc_step == TS_NOK)
		{
			rc_step = step();
		}

		switch (rc_step)
		{
		case LENTA_END:		// MFST_TRACE4("--------> LENTA_END")
			// std::cout << "------------------------------------------------------------" << std::endl;
			// sprintf_s(buf, MFST_DIAGN_MAXSIZE, "%d: всего строк %d, синтаксический анализ выполнен без ошибок", 0, lenta_size);
			// std::cout << std::setw(4) << std::left << 0 << ": всего строк" << lenta_size << ", синтаксический анализ выполнен без ошибок" << std::endl;
			rc = true;
			buildTree(idtable);
			break;
		case NS_NORULE:		// MFST_TRACE4("------->NS_NORULE")
			//std::cout << "------------------------------------------------------------" << std::endl;
			std::cout << Colors::RED << getDiagnosis(0, buf) << std::endl;
			std::cout << getDiagnosis(1, buf) << std::endl;
			std::cout << getDiagnosis(2, buf) << Colors::RESET << std::endl;
			break;
		case NS_NORULECHAIN:	// MFST_TRACE4("-------->NS_NORULECHAIN")
			break;
		case NS_ERROR:			MFST_TRACE4("-------->NS_ERROR")
			break;
		case SURPRISE:			MFST_TRACE4("-------->SURPRISE")
			break;
		};
		return rc;
	};

	char* Mfst::getCSt(char* buf)
	{
		for (int k = (signed)st.size() - 1; k >= 0; k--)
		{
			short p = st.c[k]; buf[st.size() - 1 - k] = GRB::Rule::Chain::alphabet_to_char(p);
		}
		buf[st.size()] = 0x00;
		return buf;
	}

	char* Mfst::getCLenta(char* buf, short pos, short n)
	{
		short i, k = (pos + n < lenta_size) ? pos + n : lenta_size;
		for (i = pos; i < k; i++)
		{
			buf[i - pos] = GRB::Rule::Chain::alphabet_to_char(lenta[i]);
		};
		buf[i - pos] = 0x00;
		return buf;
	}

	void Mfst::printrules()
	{
		MfstState state;
		GRB::Rule rule;
		for (unsigned short k = 0; k < storestate.size(); k++) {
			state = storestate.c[k];
			rule = grebach.getRule(state.nrule);
			char buf[1024];
			std::cout << std::setw(4) << k << std::setw(20) << rule.getCRule(buf, state.nrulechain) << std::endl;
		}
	}

	bool Mfst::buildTree(IT::IdTable& idtable)
	{
		char startSymbol = GRB::Rule::Chain::alphabet_to_char(grebach.startN);
		std::string rootName(1, startSymbol);
		tree.root = new Pt::Node(rootName);

		std::stack<Pt::Node*> nodesToProcess;
		nodesToProcess.push(tree.root);

		for (int i = 0; i < storestate.size(); i++)
		{
			MfstState state = storestate.c[i];

			GRB::Rule rule = grebach.getRule(state.nrule);
			GRB::Rule::Chain chain;
			rule.getNextChain(lenta[state.lenta_position], chain, state.nrulechain);

			if (nodesToProcess.empty()) { return false; }
			Pt::Node* parent = nodesToProcess.top();
			nodesToProcess.pop();

			std::vector<Pt::Node*> currChildren;

			for (int k = 0; k < chain.nt.size(); k++)
			{
				GRBALPHABET symbol = chain.nt[k];
				Pt::Node* child = nullptr;

				if (GRB::Rule::Chain::isT(symbol))
				{
					char val = GRB::Rule::Chain::alphabet_to_char(symbol);
					std::string symbolValue(1, val);

					short lexIdx = -1;
					if (i == 0) lexIdx = state.lenta_position;

					child = new Pt::Node(lexIdx, symbolValue);
				}
				else
				{
					char val = GRB::Rule::Chain::alphabet_to_char(symbol);
					std::string sVal(1, val);
					child = new Pt::Node(sVal);
				}

				child->parent = parent;
				parent->children.push_back(child);
				currChildren.push_back(child);
			}

			for (int i = currChildren.size() - 1; i >= 0; i--)
			{
				if (currChildren[i]->type == Pt::Node::NonTerminal)
				{
					nodesToProcess.push(currChildren[i]);
				}
			}
		}

		return true;
	}
}