#pragma once

#include "lt.h"
#include "it.h"

namespace PN
{
    int priority(LT::Entry lex);
    bool polishNotation(int lextable_pos, LT::LexTable& lextable, IT::IdTable& idtable);
}
