#include "Tree.h"
#include <iostream>
#include <cassert>

void Tree::dump(int depth)
{
	for (int i = 0; i<depth; i++)
		std::cout << "  ";

	std::cout << m_tag << ": " << m_lexeme << std::endl;
	// Recurse over the children
	for (auto child : m_children) {
		assert(child);
		child->dump(depth + 1);
	}
}