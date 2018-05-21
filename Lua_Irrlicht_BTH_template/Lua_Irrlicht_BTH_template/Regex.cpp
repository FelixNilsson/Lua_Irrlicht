#include "Regex.h"
#include <iostream>

int CharClass::match(char const* text) {
	return m_contents.find(*text) == std::string::npos ? -1 : 1;
}

int Star::match(char const* text) {
	int n;
	int consumed = 0;

	while ((n = m_operand->match(text)) > 0) {
		consumed += n;
		text += n;
	}

	return consumed;
}

int Seq::match(char const * text)
{
	int chars, consumed = 0;
	for (auto c : m_cells) {
		if ((chars = c->match(text)) < 0)  return -1;
		consumed += chars;
		text += chars;
	}
	return consumed;
}
