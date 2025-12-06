#include "ast.h"
#include <fstream>
#include <sstream>

namespace Ast
{
    void Tree::print(Node* node, int depth) {
        if (!node) return;

        for (int i = 0; i < depth; i++) std::cout << "  ";

        if (node->type == Node::NodeType::NonTerminal)
            std::cout << "<" << node->name << ">" << std::endl;
        else
            std::cout << "'" << node->name << "' idx:" << node->lexIndex << std::endl;

        for (auto child : node->children) {
            print(child, depth + 1);
        }
    }

    void Tree::writeNode(std::ofstream& ofs, Node* node) {
        if (!node) return;

        std::stringstream ss;
        ss << (long long)node;
        std::string node_id = ss.str();

        std::string label = node->name;
        // Экранирование кавычек для DOT формата
        size_t pos = 0;
        while ((pos = label.find("\"", pos)) != std::string::npos) {
            label.replace(pos, 1, "\\\"");
            pos += 2;
        }

        if (node->type == Node::NodeType::Terminal) {
            if (node->lexIndex != -1) {
                label = node->name + " (idx:" + std::to_string(node->lexIndex) + ")";
            }

            // shape=ellipse для терминалов
            ofs << node_id << " [label=\"" << label << "\", shape=ellipse, style=filled, fillcolor=\"#E6E6FA\"];\n";
        }
        else {
            // Нетерминалы: shape=box
            label = "<" + label + ">";
            ofs << node_id << " [label=\"" << label << "\", shape=box, style=filled, fillcolor=\"#ADD8E6\"];\n";
        }

        for (auto child : node->children) {
            std::stringstream child_ss;
            child_ss << (long long)child;
            std::string child_id = child_ss.str();

            ofs << node_id << " -> " << child_id << ";\n";
            writeNode(ofs, child);
        }
    }

    bool Tree::toDot(const char* filename)
    {
        if (!root) return false;

        std::ofstream ofs(filename);
        if (!ofs.is_open()) return false;

        ofs << "digraph AST {\n";
        ofs << "  rankdir=TB;\n";
        ofs << "  node [fontname=\"Arial\"];\n";
        ofs << "  edge [fontname=\"Arial\"];\n\n";

        writeNode(ofs, root);

        ofs << "}\n";
        ofs.close();

        return true;
    }
}