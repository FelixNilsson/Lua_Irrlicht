#pragma once

#include <string>
#include <list>

class Tree
{
public:
	std::string m_tag;
	std::string m_lexeme;
	std::list<Tree*> m_children;
public:
	Tree(std::string t, char* l, int size) : m_tag(t), m_lexeme(l, size) {}
	~Tree() { for (auto c : m_children) delete c; }

	void dump(int depth = 0);
};