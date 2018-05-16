#pragma once

#include "Regex.h"
#include "Tree.h"

class SceneParser
{
private:
	char* m_input;
	Tree* root;

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
	bool FILE(Tree** = nullptr);
	bool FUNCTION(Tree** = nullptr);
	bool MESH(Tree** = nullptr);
	bool TEXTURE(Tree** = nullptr);
	bool SCENE(Tree** = nullptr);
	bool MBODY(Tree** tree);
	bool TRIANGLES(Tree** tree);
	bool TRIANGLE(Tree** tree);
	bool VECTOR3(Tree** tree);
	bool TSEPERATOR(Tree** tree);
	bool NUMBER(Tree** tree);
	bool SBODY(Tree** tree);
	bool SFUNCTIONS(Tree** tree);

	bool TERM(const char *lit, Tree** = nullptr);
	//bool NUM(Tree** = nullptr);
	//bool TABLE2(Tree** = nullptr);
	//bool LIST(Tree** = nullptr);
	bool WORD(Tree** = nullptr);
	bool STRING(Tree** = nullptr);
	bool DEC(Tree** = nullptr);
	//bool HEX(Tree** = nullptr);
	//bool KEY(Tree** = nullptr);
	//bool VALUE(Tree** = nullptr);
	//bool FIELDSEP(Tree **result = nullptr);
	//bool TABLE(Tree** tree = nullptr);
public:
	SceneParser(char*);
	~SceneParser();
};
