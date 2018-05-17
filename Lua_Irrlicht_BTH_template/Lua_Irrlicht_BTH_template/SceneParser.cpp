#include "SceneParser.h"
#include <iostream>

// FILE:		FUNCTION*
// FUNCTION:	MESH | TEXTURE | SCENE
// MESH:		"Mesh(" STRING ")" MBODY
// MBODY:		"\n{\n" TRIANGLES "}\n"
// TRIANGLES:	TRIANGLE*
// TRIANGLE:	VECTOR3 ",\n" VECTOR3 ",\n" VECTOR3
// VECTOR3:		"(" NUMBER "," NUMBER "," NUMBER ")"
// NUMBER:		"-" [0-9]* | [0-9]*
// SCENE:		"Scene()" SBODY
// SBODY:		"\n{\n" SFUNCTIONS "}"
// SFUNCTIONS:	"Mesh(" STRING ")\n" SFUNCTIONS | EMPTY
// SFUNCTIONS:	NAME "(" STRING ")\n" SFUNCTIONS | EMPTY
// STRING:		'\"' [a-z]+ '\"'


// TSEPERATOR:	",\n" TRIANGLE | "\n" UNUSED

SceneParser::SceneParser(char* input) : m_input(input), digit("0123456789"), hex("0123456789abcdefABCDEF"), nonzero("123456789"), dot("."),
letters("QWERTYUIOPASDFGHJKLMNBVCXZqwertyuiopasdfghjklzxcvbnm"), sDigit(&digit), number({ &nonzero, &sDigit }), zero("0"), x("x"), sHex(&hex),
hexnumber({ &zero, &x, &hex, &sHex }), sLetters(&letters), word({ &letters, &sLetters }) {
	if (FILE(&m_root)) {
		m_root->dump();
	}
	else
		std::cout << "Wrong with parsing" << std::endl;

}


SceneParser::~SceneParser() {
}

void SceneParser::buildScene(lua_State* L) const {
	Tree* tree = m_root->m_children.back();
	tree = tree->m_children.back();

	if (tree->m_tag != "SCENE") {
		std::cout << "no scene function" << std::endl;
		return;
	}

	tree = tree->m_children.back();
	tree = tree->m_children.back();

}

bool SceneParser::FILE(Tree** tree) {// FILE: FUNCTION*
	/*Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	char* start = m_input;
	if (FUNCTION(&child1) && FILE(&child2)) {
		*tree = new Tree("FILE", start, m_input - start);
		(*tree)->m_children.push_back(child1);
		if (child2)
			(*tree)->m_children.push_back(child2);
	}
	*/
	Tree* child = nullptr;
	std::list<Tree*> list;
	char* start = m_input;
	while (FUNCTION(&child)) {
		list.push_back(child);
	}
	if (list.size() > 0) {
		*tree = new Tree("FILE", start, m_input - start);
		(*tree)->m_children = list;
	}
	if (*m_input != '\0') {
		std::cout << "Warning bad file format" << std::endl;
	}

	return true;
}

bool SceneParser::FUNCTION(Tree** tree) {
	Tree *child = nullptr;
	char* start = m_input;
	if (MESH(&child) || TEXTURE(&child) || SCENE(&child)) {
		*tree = new Tree("FUNCTION", start, m_input - start);
		(*tree)->m_children.push_back(child);
		return true;
	}
	return false;
}

bool SceneParser::MESH(Tree** tree) {// MESH:	"Mesh(" STRING ")" MBODY
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	Tree* child3 = nullptr;
	Tree* child4 = nullptr;
	char* start = m_input;

	if (TERM("Mesh(", &child1) && STRING(&child2) && TERM(")", &child3) && MBODY(&child4)) {
		*tree = new Tree("MESH", start, m_input - start);
		//(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);
		//(*tree)->m_children.push_back(child3);
		(*tree)->m_children.push_back(child4);

		return true;
	}

	return false;
}

bool SceneParser::TEXTURE(Tree** tree) {//"(" NUMBER "," NUMBER "," NUMBER ")"
	
	
	return false;
}

bool SceneParser::SCENE(Tree** tree) {// SCENE:	"Scene()" SBODY
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	char* start = m_input;

	if (TERM("Scene()", &child1) && SBODY(&child2)) {
		*tree = new Tree("SCENE", start, m_input - start);
		//(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);

		return true;
	}

	return false;
}

bool SceneParser::MBODY(Tree** tree) {// MBODY:	"\n{\n" TRIANGLES "}\n"
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	Tree* child3 = nullptr;
	char* start = m_input;

	if (TERM("\n{\n", &child1) && TRIANGLES(&child2) && TERM("}\n", &child3)) {
		*tree = new Tree("MBODY", start, m_input - start);
		//(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);
		//(*tree)->m_children.push_back(child3);

		return true;
	}

	return false;
}

bool SceneParser::TRIANGLES(Tree** tree) {// TRIANGLES:	TRIANGLE*
	Tree* child = nullptr;
	std::list<Tree*> list;
	char* start = m_input;
	bool comma = true;

	while (TRIANGLE(&child)) {
		list.push_back(child);

		if (!TERM(",\n", &child)) {
			comma = false;
			break;
		}
	}

	if (!comma && !TERM("\n", &child)) {
		return false;
	}

	if (list.size() > 0) {
		*tree = new Tree("TRIANGLES", start, m_input - start);
		(*tree)->m_children = list;
	}

	return true;
}

bool SceneParser::TRIANGLE(Tree** tree) {// TRIANGLE:	VECTOR3 ",\n" VECTOR3 ",\n" VECTOR3
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	Tree* child3 = nullptr;
	Tree* child4 = nullptr;
	Tree* child5 = nullptr;
	char* start = m_input;

	if (VECTOR3(&child1) && TERM(",\n", &child2) && VECTOR3(&child3) && TERM(",\n", &child4) && VECTOR3(&child5)) {
		*tree = new Tree("TRIANGLE", start, m_input - start);
		(*tree)->m_children.push_back(child1);
		//(*tree)->m_children.push_back(child2);
		(*tree)->m_children.push_back(child3);
		//(*tree)->m_children.push_back(child4);
		(*tree)->m_children.push_back(child5);

		return true;
	}

	return false;
}

bool SceneParser::VECTOR3(Tree** tree) {//"(" NUMBER "," NUMBER "," NUMBER ")"
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	Tree* child3 = nullptr;
	Tree* child4 = nullptr;
	Tree* child5 = nullptr;
	Tree* child6 = nullptr;
	Tree* child7 = nullptr;
	char* start = m_input;
	if (TERM("(", &child1) && NUMBER(&child2) && TERM(",", &child3) && NUMBER(&child4) && TERM(",", &child5) && NUMBER(&child6) && TERM(")", &child7)) {
		*tree = new Tree("VECTOR3", start, m_input - start);
		//(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);
		//(*tree)->m_children.push_back(child3);
		(*tree)->m_children.push_back(child4);
		//(*tree)->m_children.push_back(child5);
		(*tree)->m_children.push_back(child6);
		//(*tree)->m_children.push_back(child7);

		return true;
	}
	return false;
}

bool SceneParser::TSEPERATOR(Tree** tree) {// TSEPERATOR: ",\n" TRIANGLE | "\n"
	/*Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	char* start = m_input;

	if (TERM(",\n", &child1) && TRIANGLE(&child2)) {
		*tree = new Tree("TSEPERATOR", start, m_input - start);
		(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);
		return true;
	}
	if (TERM("\n", &child1)) {
		*tree = new Tree("TSEPERATOR", start, m_input - start);
		(*tree)->m_children.push_back(child1);

		return true;
	}*/

	return false;
}

bool SceneParser::NUMBER(Tree** tree) {//"-"[0 - 9] * | [0 - 9] *
	char *start = m_input;
	Tree *child = nullptr;

	if (TERM("-", &child)) {
		int nr = sDigit.match(m_input);
		if (nr > 0) {
			m_input += nr;
			*tree = new Tree("NUMBER", start, m_input - start);
			return true;
		}
	}

	else {
		int nr = sDigit.match(m_input);
		if (nr > 0) {
			m_input += nr;
			*tree = new Tree("NUMBER", start, m_input - start);
			return true;
		}
	}

	return false;
}


bool SceneParser::SBODY(Tree** tree) {// SBODY:	"\n{\n" SFUNCTIONS "}"
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	Tree* child3 = nullptr;
	char *start = m_input;

	if (TERM("\n{\n", &child1) && SFUNCTIONS(&child2) && TERM("}", &child3)) {
		*tree = new Tree("SBODY", start, m_input - start);
		//(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);
		//(*tree)->m_children.push_back(child3);
		return true;
	}
	return false;
}

bool SceneParser::SFUNCTIONS(Tree** tree) {// SFUNCTIONS: "Mesh(" STRING ")\n" SFUNCTIONS | EMPTY
	Tree *child1 = nullptr, *child2 = nullptr, *child3 = nullptr, *child4 = nullptr;
	char* start = m_input;
	if (TERM("Mesh(", &child1) && STRING(&child2) && TERM(")\n", &child3) && SFUNCTIONS(&child4)) {
		*tree = new Tree("SFUNCTIONS", start, m_input - start);
		if (child1)
			//(*tree)->m_children.push_back(child1);
		if (child2)
			(*tree)->m_children.push_back(child2);
		if (child3)
			//(*tree)->m_children.push_back(child3);
		if (child4)
			(*tree)->m_children.push_back(child4);
	}
	return true;
}

/*bool SceneParser::FIELDSEP(Tree **result) {  // FIELDSEP : "," | ";"
	Tree *child1;
	char *start = m_input;
	if (!TERM(",", &child1) ||
		TERM(";", &child1))   return false;
	*result = new Tree("FIELDSEP", start, m_input - start);
	(*result)->m_children.push_back(child1);
	return true;
}*/

/*bool SceneParser::TABLE(Tree** tree) {
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
}*/

/*bool SceneParser::TABLE2(Tree** tree) {
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
}*/

/*bool SceneParser::LIST(Tree** tree) {
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
}*/

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
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	Tree* child3 = nullptr;
	char* start = m_input;

	if (TERM("\"", &child1) && WORD(&child2) && TERM("\"", &child3)) {
		*tree = new Tree("STRING", start, m_input - start);
		//(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);
		//(*tree)->m_children.push_back(child3);

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

/*bool SceneParser::HEX(Tree** tree) {
	char* start = m_input;
	int n;
	if ((n = hexnumber.match(m_input)) < 0)
		return false;

	m_input += n;
	*tree = new Tree("HEX", start, m_input - start);

	return true;
}*/

/*bool SceneParser::KEY(Tree** tree) {
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
}*/

/*bool SceneParser::VALUE(Tree** tree) {
	Tree* child1;
	char* start = m_input;

	if (TABLE(&child1) || NUM(&child1) || STRING(&child1)) {
		*tree = new Tree("VALUE", start, m_input - start);
		(*tree)->m_children.push_back(child1);

		return true;
	}

	return false;
}*/

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

/*bool SceneParser::NUM(Tree** tree) {
	Tree* child1;
	char* start = m_input;

	if (HEX(&child1) || DEC(&child1)) {
		*tree = new Tree("NUM", start, m_input - start);
		(*tree)->m_children.push_back(child1);

		return true;
	}

	return false;
}*/