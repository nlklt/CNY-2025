#pragma once

#include "lt.h"
#include "it.h"

#include <queue>

namespace PN
{
	bool polishNotation(int lextable_pos, LT::LexTable& lextable, IT::IdTable& idtable);
	int priority(char v);
}