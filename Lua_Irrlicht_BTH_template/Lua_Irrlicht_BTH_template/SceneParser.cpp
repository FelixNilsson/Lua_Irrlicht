#include "SceneParser.h"

// FILE:		FUNCTION FILE | EMPTY
// FUNCTION:	FNAME "(" ARGUMENT ")" WS* "{" BODY "}"
// FUNCTION:	MESH | TEXTURE | SCENE
// MESH:		"Mesh(" STRING ")" MBODY
// MBODY:		"{" TRIANGLES "}"
// TRIANGLES:	TRIANGLE*
// TRIANGLE:	VECTOR3 "," VECTOR3 "," VECTOR3 TSEPERATOR
// VECTOR3:		"(" NUMBER "," NUMBER "," NUMBER ")"
// TSEPERATOR:	",\n" TRIANGLE | EMPTY
// NUMBER:		"-" [0-9]* | [0-9]*
// SCENE:		"Scene()" SBODY
// SBODY:		"{" SFUNCTIONS "}"
// SFUNCTIONS:	"Mesh(" STRING ")" SFUNCTIONS | EMPTY
// STRING:		'\"' [a-z]+ '\"'

SceneParser::SceneParser(char* input) : m_input(input), digit("0123456789"), hex("0123456789abcdefABCDEF"), nonzero("123456789"), dot("."),
letters("QWERTYUIOPASDFGHJKLMNBVCXZqwertyuiopasdfghjklzxcvbnm"), sDigit(&digit), number({ &nonzero, &sDigit }), zero("0"), x("x"), sHex(&hex),
hexnumber({ &zero, &x, &hex, &sHex }), sLetters(&letters), word({ &letters, &sLetters }) {
}


SceneParser::~SceneParser() {
}

bool SceneParser::FIELDSEP(Tree **result) {  // FIELDSEP : "," | ";"
	Tree *child1;
	char *start = m_input;
	if (!TERM(",", &child1) ||
		TERM(";", &child1))   return false;
	*result = new Tree("FIELDSEP", start, m_input - start);
	(*result)->m_children.push_back(child1);
	return true;
}

bool SceneParser::TABLE(Tree** tree) {
	Tree* child1;
	Tree* child2;
	Tree* child3 = nullptr;
	char* start = m_input;

	if (TERM("{", &child1) && TABLE2(&child3) && TERM("}", &child2)) {
		*tree = new Tree("TABLE", start, m_input - start);
		(*tree)->m_children.push_back(child1);
		if (child3)
			(*tree)->m_children.push_back(child3);
		(*tree)->m_children.push_back(child2);

		return true;
	}

	return false;
}

bool SceneParser::TABLE2(Tree** tree) {
	Tree* child1;
	char* start = m_input;

	if (TABLE(&child1) || NUM(&child1) || STRING(&child1) || KEY(&child1)) {
		*tree = new Tree("TABLE2", start, m_input - start);
		(*tree)->m_children.push_back(child1);

		child1 = nullptr;
		if (LIST(&child1)) {
			//*tree = new Tree("TABLE2", start, input - start);
			if (child1)
				(*tree)->m_children.push_back(child1);

			return true;
		}
		else {
			return false;
		}
	}


	return true;
}

bool SceneParser::LIST(Tree** tree) {
	Tree* child1;
	char* start = m_input;

	if (TERM(",", &child1) || TERM(";", &child1)) {
		*tree = new Tree("LIST", start, m_input - start);
		(*tree)->m_children.push_back(child1);

		child1 = nullptr;
		if (TABLE2(&child1)) {
			if (child1)
				(*tree)->m_children.push_back(child1);

			return true;
		}
		else {
			return false;
		}
	}

	return true;
}

bool SceneParser::WORD(Tree** tree) {
	char* start = m_input;

	int n;
	if ((n = word.match(m_input)) < 0)
		return false;

	m_input += n;
	*tree = new Tree("WORD", start, m_input - start);

	return true;
}

bool SceneParser::STRING(Tree** tree) {
	Tree* child1;
	Tree* child2;
	Tree* child3;
	char* start = m_input;

	if (TERM("\"", &child1) && WORD(&child2) && TERM("\"", &child3)) {
		*tree = new Tree("STRING", start, m_input - start);
		(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);
		(*tree)->m_children.push_back(child3);

		return true;
	}

	return false;
}

bool SceneParser::DEC(Tree** tree) {
	char* start = m_input;
	int n;
	if ((n = number.match(m_input)) < 0)
		return false;

	m_input += n;
	*tree = new Tree("DEC", start, m_input - start);

	return true;
}

bool SceneParser::HEX(Tree** tree) {
	char* start = m_input;
	int n;
	if ((n = hexnumber.match(m_input)) < 0)
		return false;

	m_input += n;
	*tree = new Tree("HEX", start, m_input - start);

	return true;
}

bool SceneParser::KEY(Tree** tree) {
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	Tree* child3 = nullptr;
	char* start = m_input;

	if ((WORD(&child1) && TERM("=", &child2)) || (TERM("[", &child1) && (WORD(&child2) || STRING(&child2) || NUM(&child2)) && TERM("]=", &child3))) {
		*tree = new Tree("KEY", start, m_input - start);
		(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);

		if (child3)
			(*tree)->m_children.push_back(child3);

		child1 = nullptr;
		if (VALUE(&child1)) {
			(*tree)->m_children.push_back(child1);

			return true;
		}

		return false;
	}

	return false;
}

bool SceneParser::VALUE(Tree** tree) {
	Tree* child1;
	char* start = m_input;

	if (TABLE(&child1) || NUM(&child1) || STRING(&child1)) {
		*tree = new Tree("VALUE", start, m_input - start);
		(*tree)->m_children.push_back(child1);

		return true;
	}

	return false;
}

bool SceneParser::TERM(const char *lit, Tree** tree)
{
	int i;
	char* start = m_input;
	for (i = 0; lit[i] != 0; i++)
		if (m_input[i] != lit[i])
			return false;
	m_input += i;

	*tree = new Tree("TERM", start, m_input - start);

	return true;
}

bool SceneParser::EXPR(Tree** tree) {
	return (NUM() && OP() && NUM())
		|| (TERM("(") && EXPR() && TERM(")"));
}

bool SceneParser::OP(Tree** tree) {
	return (TERM("+") || TERM("-") ||
		TERM("/") || TERM("*"));
}

bool SceneParser::NUM(Tree** tree) {
	Tree* child1;
	char* start = m_input;

	if (HEX(&child1) || DEC(&child1)) {
		*tree = new Tree("NUM", start, m_input - start);
		(*tree)->m_children.push_back(child1);

		return true;
	}

	return false;
}