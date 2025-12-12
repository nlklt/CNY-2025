#pragma once

#include "ast.h"
#include "it.h"
#include "lt.h"
#include "error.h"

namespace SM
{
    void semanticAnalysis(LT::LexTable& lextable, IT::IdTable& idtable);
    IT::IDDATATYPE getTypeFromLexEntry(const LT::Entry& lex, IT::IdTable& idtable);
}
