#pragma once
#include "ast.h"
#include "it.h"
#include "lt.h"
#include "error.h"

namespace SM
{
	// Основная функция запуска анализа
	bool analyze(Ast::Tree& tree, IT::IdTable& idtable);
	void semanticAnalysis(LT::LexTable& lextable, IT::IdTable& idtable);
	IT::IDDATATYPE evaluateRPN(int pos, LT::LexTable&, IT::IdTable&);
	// Внутренняя функция проверки конкретного узла
	void checkNode(Ast::Node* node, IT::IdTable& idtable);
}