#pragma once
#include "lt.h"
#include "it.h"

namespace Polis {
	int findExpression(LT::LexTable& lextable);
	bool polishNotation(int lextable_pos, LT::LexTable& lextable, IT::IdTable& idtable);
	int priority(char v);
}