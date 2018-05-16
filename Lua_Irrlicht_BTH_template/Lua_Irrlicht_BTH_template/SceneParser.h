#pragma once

#include "Regex.h"
#include "Tree.h"

class SceneParser
{
private:
	char* m_input;
	Tree* tree;

	CharClass digit;
	CharClass hex;
	CharClass nonzero;
	CharClass dot;
	CharClass letters;
	Star sDigit;
	Seq number;
	CharClass zero;
	CharClass x;
	Star sHex;
	Seq hexnumber;
	Star sLetters;
	Seq word;
private:
	bool TERM(const char *lit, Tree** = nullptr);
	bool EXPR(Tree** = nullptr);
	bool OP(Tree** = nullptr);
	bool NUM(Tree** = nullptr);
	bool TABLE2(Tree** = nullptr);
	bool LIST(Tree** = nullptr);
	bool WORD(Tree** = nullptr);
	bool STRING(Tree** = nullptr);
	bool DEC(Tree** = nullptr);
	bool HEX(Tree** = nullptr);
	bool KEY(Tree** = nullptr);
	bool VALUE(Tree** = nullptr);
	bool FIELDSEP(Tree **result = nullptr);
	bool TABLE(Tree** tree = nullptr);
public:
	SceneParser(char*);
	~SceneParser();
};

