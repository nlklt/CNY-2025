#pragma once

#include "in.h"
#include "lt.h"
#include "it.h"

namespace Lexer
{
    void BaseToDecimal(char*, int);
    void lexicalAnalysis(In::IN& in, LT::LexTable& lt, IT::IdTable& it);
}