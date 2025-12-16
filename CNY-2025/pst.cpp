#include "pst.h"
#include <fstream>
#include <sstream>

namespace Pt
{
    bool first = true;
    void Tree::print(Node* node, int depth, std::ofstream* log) {
        if (!node) return;
        if (first) {
            *log << "\n================ Дерево разбора ================" << std::endl;
        }

        for (int i = 0; i < depth; i++) *log << "  ";

        if (node->type == Node::NodeType::NonTerminal)
            *log << "<" << node->name << ">" << std::endl;
        else
            *log << "'" << node->name << "' idx:" << node->lexIndex << std::endl;

        for (auto child : node->children) {
            print(child, depth + 1, log);
        }
        if (first) {
            *log << "================================================" << std::endl;
        }
        first = false;
    }

    void Tree::writeNode(std::ofstream& ofs, Node* node) {
        if (!node) return;

        std::stringstream ss;
        ss << (long long)node;
        std::string node_id = ss.str();

        std::string label = node->name;
        size_t pos = 0;
        while ((pos = label.find("\"", pos)) != std::string::npos) {
            label.replace(pos, 1, "\\\"");
            pos += 2;
        }

        if (node->type == Node::NodeType::Terminal) {
            if (node->lexIndex != -1) {
                label = node->name;
            }

            ofs << node_id << " [label=\"" << label << "\", shape=circle, style=filled, fillcolor=\"#FFFFFF\"];\n";
        }
        else {
            label = label;
            ofs << node_id << " [label=\"" << label << "\", shape=circle, style=filled, fillcolor=\"#FFFFFF\"];\n";
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
        ofs << "digraph PST {\n";
        ofs << "  rankdir=TB;\n";


        ofs << "  splines=polyline;\n";

        ofs << "  node [\n";
        ofs << "    fontname=\"Helvetica Bold\", \n";
        ofs << "    fontsize=24, \n";
        ofs << "    shape=circle,\n";
        ofs << "    fixedsize=true,\n";
        ofs << "    width=1.0,\n";
        ofs << "    height=1.0\n";
        ofs << "  ];\n\n";

        ofs << "  edge [\n";
        ofs << "    fontname=\"Helvetica\", \n";
        ofs << "    style=bold\n";
        ofs << "  ];\n\n";

        writeNode(ofs, root);

        ofs << "}\n";
        ofs.close();

        writeNode(ofs, root);

        ofs << "}\n";
        ofs.close();

        return true;
    }
}