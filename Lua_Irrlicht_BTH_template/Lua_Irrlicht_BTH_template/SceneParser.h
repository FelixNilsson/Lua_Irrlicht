#pragma once

#include <lua.hpp>
#include "Regex.h"
#include "Tree.h"

class SceneParser
{
private:
	char* m_input;
	Tree* m_root = nullptr;
	int m_counter = 1;

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
	void buildMesh(lua_State* L, std::string& arg, const char *p = nullptr);
	void buildTexture(lua_State* L);
	void addTexture(lua_State* L, Tree* tree);

	bool FILE(Tree** = nullptr);
	bool FUNCTION(Tree** = nullptr);
	bool MESH(Tree** = nullptr);
	bool TEXTURE(Tree** = nullptr);
	bool TBODY(Tree** tree);
	bool ROWS(Tree** tree);
	bool ROW(Tree** tree);
	bool LUA(Tree** tree);
	bool LBODY(Tree** tree);
	bool CODE(Tree** tree);
	bool WHITESPACE();
	bool SCENE(Tree** = nullptr);
	bool MBODY(Tree** tree);
	bool TRIANGLES(Tree** tree);
	bool TRIANGLE(Tree** tree);
	bool TRIANGLE5(Tree** tree);
	bool VECTOR3(Tree** tree);
	bool VECTOR5(Tree** tree);
	bool NUMBER(Tree** tree);
	bool SBODY(Tree** tree);
	bool SFUNCTIONS(Tree** tree);
	bool TRANSFORM(Tree** tree);
	bool BIND(Tree** tree);
	bool SMESH(Tree** tree);

	bool TERM(const char *lit);
	bool WORD(Tree** = nullptr);
	bool STRING(Tree** = nullptr);
public:
	SceneParser(char*);
	~SceneParser();

	void buildScene(lua_State* L);

};

