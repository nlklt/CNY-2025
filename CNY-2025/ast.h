#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "lt.h"
#include "it.h"

namespace Ast
{
	struct Node
	{
		enum NodeType { NonTerminal, Terminal } type;

		std::string name;		// Имя нетерминала
		short lexIndex;			// Индекс в таблице лексем (для Terminal) : первое вхождение
		IT::IDDATATYPE dataType;		// Для семантического анализа

		std::vector<Node*> children; // Потомки
		Node* parent;				 // Родитель

		// Конструктор для нетерминала
		Node(std::string n) : type(NonTerminal), name(n), lexIndex(-1), parent(nullptr), dataType(IT::IDDATATYPE::UNDEF) {}

		// Конструктор для терминала
		Node(short lexIdx, std::string val) : type(Terminal), lexIndex(lexIdx), name(val), parent(nullptr), dataType(IT::IDDATATYPE::UNDEF) {}
	};

	struct Tree
	{
		Node* root;

		Tree() : root(nullptr) {}

		// Печать дерева
		void print(Node* node, int depth = 0);
		bool toDot(const char* filename);
	private:
		void writeNode(std::ofstream& ofs, Node* node);
	};
}