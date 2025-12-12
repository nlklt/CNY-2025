#pragma once
#include "lt.h"
#include "it.h"
#include "polis.h"

#include <iostream>
#include <stack>

#define BEGIN	".586\n						\
.model flat, stdcall\n						\
includelib kernel32.lib\n					\
includelib libucrt.lib\n					\
ExitProcess PROTO : DWORD\n					\
.stack 4096\n"
#define DATA_SEG ".data\n"
#define CODE_SEG ".code\n"


namespace GN
{

	enum LabelType { L_IF, L_SWITCH, L_CASE, L_DEFAULT };

	struct LabelInfo {
		int id;			    
		LabelType type;     
		int parentSwitchID; 
	};

	void Generate(LT::LexTable& lextable, IT::IdTable& idtable, std::ostream* stream);
}