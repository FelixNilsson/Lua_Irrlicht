#include "SceneParser.h"
#include <iostream>

// FILE:		FUNCTION* | SCENE
// FUNCTION:	MESH | LUA | TEXTURE 
// MESH:		"Mesh(" STRING ")" MBODY
// MBODY:		"\n{\n" TRIANGLES "}\n"
// TRIANGLES:	TRIANGLE* | TRIANGLE5*
// TRIANGLE:	VECTOR3 ",\n" VECTOR3 ",\n" VECTOR3
// TRIANGLE5:	VECTOR5 ",\n" VECTOR5 ",\n" VECTOR5
// VECTOR3:		"(" NUMBER "," NUMBER "," NUMBER ")"
// VECTOR5:		"(" NUMBER "," NUMBER "," NUMBER "," NUMBER "," NUMBER")"
// NUMBER:		"-" [0-9]* | [0-9]*
// SCENE:		"Scene()" SBODY
// SBODY:		"\n{\n" SFUNCTIONS* "}"
// SFUNCTIONS:	SMESH | BIND
// SMESH:		"Mesh" "(" STRING ")\n"
// BIND:		"Bind" "(" STRING ", " SMESH ")\n"
// STRING:		'\"' [a-z]+ '\"'
// TEXTURE:		"Texture(" STRING ")\n" TBODY
// TBODY:		"{\n" ROWS "}\n"
// ROWS:		ROW*
// ROW:			(VECTOR3 "," VECTOR3)* RSEPERATOR
// RSEPERATOR:	",\n" (ROW | EMPTY) | "\n"
// LUA:			"Mesh(" STRING ")\n" LBODY
// LBODY:		"Lua(<\n" CODE ">)\n"
// CODE:		[^">)"]*
// WHITESPACE:	['\t'' ''\n']*


// TSEPERATOR:	",\n" TRIANGLE | "\n" UNUSED

SceneParser::SceneParser(char* input) : m_input(input), digit("0123456789"), hex("0123456789abcdefABCDEF"), nonzero("123456789"), dot("."),
letters("QWERTYUIOPASDFGHJKLMNBVCXZqwertyuiopasdfghjklzxcvbnm"), sDigit(&digit), number({ &nonzero, &sDigit }), zero("0"), x("x"), sHex(&hex),
hexnumber({ &zero, &x, &hex, &sHex }), sLetters(&letters), word({ &letters, &sLetters, &sDigit }) {
	if (FILE(&m_root)) {
		m_root->dump();
		std::cout << "Succes with parsing" << std::endl;
	}
	else
		std::cout << "Wrong with parsing" << std::endl;

}


SceneParser::~SceneParser() {
}

void SceneParser::buildScene(lua_State* L) const {
	if (m_root == nullptr)
		return;


	Tree* tree = m_root->m_children.back();
	//tree = tree->m_children.back();

	if (tree->m_tag != "SCENE") {
		std::cout << "no scene function" << std::endl;
		return;
	}

	buildTexture(L);
	tree = tree->m_children.back();

	for (auto p : tree->m_children) {

		if (p->m_tag == "Mesh") {
			std::cout << p->m_children.front()->m_lexeme << std::endl;
			buildMesh(L, p->m_children.front()->m_lexeme);
		}
		else if (p->m_tag == "Bind") {
			std::cout << "bind" << std::endl;
			std::list<Tree*>::iterator it = p->m_children.end();
			it--;
			buildMesh(L, (*it)->m_lexeme);
			lua_getglobal(L, "bind");
			lua_pushstring(L, (*it)->m_lexeme.c_str());
			it--;
			lua_pushstring(L, (*it)->m_lexeme.c_str());
			lua_pcall(L, 2, 0, 0);
		}
		else
			std::cout << "nay" << std::endl;


		std::cout << p->m_lexeme << std::endl;
	}

	tree = tree->m_children.back();

}

void SceneParser::buildMesh(lua_State* L, std::string& arg) const {
	Tree* tree = nullptr;
	bool code = false;

	for (auto p : m_root->m_children) {
		//Tree* temp = p->m_children.front();
		if (p->m_tag == "MESH" && (p->m_children.front()->m_lexeme == arg)) {
			tree = p->m_children.back();//tree points to triangles
			break;
		}
		else if (p->m_tag == "LUA" && (p->m_children.front()->m_lexeme == arg)) {
			tree = p->m_children.back();//tree points to lua code
			code = true;
			break;
		}
	}

	if (!tree)
		return;

	//tree = tree->m_children.front();
	if (code) {
		lua_getglobal(L, "addMesh");//1
		
		if (luaL_loadstring(L, "local args = {...} print(\"hello\") tri = {} for k, v in pairs(args[1]) do tri[k] = {args[2](v[1],v[2],v[3],v[4],v[5])} end return tri")) {//2
			std::cout << lua_tostring(L, -1) << '\n';
			lua_pop(L, 1);
		}
		
		if (luaL_loadstring(L, tree->m_lexeme.c_str()) || lua_pcall(L, 0, 1, 0)) {//3
			std::cout << lua_tostring(L, -1) << '\n';
			lua_pop(L, 1);
		}

		if (luaL_loadstring(L, "local args = {...} return args[1]+10, args[2], args[3], args[4], args[5]")) {//2
			std::cout << lua_tostring(L, -1) << '\n';
			lua_pop(L, 1);
		}
		if (/*luaL_loadstring(L, "local args = {...} return args[1]+10, args[2], args[3], args[4], args[5]") ||*/ lua_pcall(L, 2, 1, 0)) {
			std::cout << lua_tostring(L, -1) << '\n';
			lua_pop(L, 1);
		}
		
		lua_pushstring(L, arg.c_str());
		if (lua_pcall(L, 2, 0, 0)) {
			std::cout << lua_tostring(L, -1) << '\n';
			lua_pop(L, 1);
		}

		return;
	}
	// loop over all triangles
	std::vector<int> con;
	lua_getglobal(L, "addMesh");
	lua_newtable(L);
	int index1 = 1;
	for (auto p : tree->m_children) {
		for (auto vector : p->m_children) {
			lua_newtable(L);
			int index2 = 1;
			for (auto number : vector->m_children) {
				con.push_back(std::stoi(number->m_lexeme));
				lua_pushnumber(L, std::stoi(number->m_lexeme));
				lua_rawseti(L, -2, index2);
				index2++;
			}
			lua_rawseti(L, -2, index1);
			index1++;
		}
	}
	lua_pushstring(L, arg.c_str());
	lua_pcall(L, 2, 0, 0);

	std::cout << "debug\n";
}

void SceneParser::buildTexture(lua_State* L) const {
	//Tree* tree = nullptr;

	for (auto p : m_root->m_children) {
		//Tree* temp = p->m_children.front();
		if (p->m_tag == "TEXTURE") {
			addTexture(L, p);
		}
	}


}

void SceneParser::addTexture(lua_State* L, Tree* tree) const {
	std::string name = tree->m_children.front()->m_lexeme;
	tree = tree->m_children.back(); //rows
	
	
	std::vector<std::vector<std::vector<int>>> debug_row;
	lua_getglobal(L, "addTexture");
	lua_newtable(L);
	int rowNr = 1;
	for (auto row : tree->m_children) {
		lua_newtable(L);
		int vectorNr = 1;
		std::vector<std::vector<int>> debug_vector;
		for (auto vector : row->m_children) {
			lua_newtable(L);
			int index = 1;
			std::vector<int> con;
			for (auto number : vector->m_children) {
				con.push_back(std::stoi(number->m_lexeme));
				lua_pushnumber(L, std::stoi(number->m_lexeme));
				lua_rawseti(L, -2, index);
				index++;
			}
			debug_vector.push_back(con);
			lua_rawseti(L, -2, vectorNr);
			vectorNr++;
		}
		debug_row.push_back(debug_vector);
		lua_rawseti(L, -2, rowNr);
		rowNr++;
	}
	lua_pushstring(L, name.c_str());
	lua_pcall(L, 2, 0, 0);
}

bool SceneParser::FILE(Tree** tree) {// FILE: FUNCTION* | SCENE
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
	while (WHITESPACE() && FUNCTION(&child)) {
		list.push_back(child);
	}
	WHITESPACE();
	if (SCENE(&child)) {
		list.push_back(child);
	}

	if (list.size() > 0) {
		*tree = new Tree("FILE", start, m_input - start);
		(*tree)->m_children = list;
	}
	if (*m_input != '\0') {
		std::cout << "Warning bad file format" << std::endl;
		return false;
	}

	return true;
}

bool SceneParser::FUNCTION(Tree** tree) {// FUNCTION: MESH | LUA | TEXTURE 
	Tree *child = nullptr;
	char* start = m_input;
	if (MESH(&child) || TEXTURE(&child) || LUA(&child)) {
		//*tree = new Tree("FUNCTION", start, m_input - start);
		//(*tree)->m_children.push_back(child);

		*tree = child;

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
	m_input = start;

	return false;
}

bool SceneParser::TEXTURE(Tree** tree) {// TEXTURE:	"Texture(" STRING ")\n" TBODY
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	
	char* start = m_input;

	if (TERM("Texture(", &child1) && STRING(&child1) && TERM(")\n", &child2) && TBODY(&child2)) {
		*tree = new Tree("TEXTURE", start, m_input - start);
		(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);

		return true;
	}

	return false;
}

bool SceneParser::TBODY(Tree** tree) {// TBODY:	"{\n" ROWS "}\n"
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	Tree* child3 = nullptr;
	char* start = m_input;

	if (TERM("{", &child1) && WHITESPACE() && ROWS(&child2) && TERM("}\n", &child3)) {
		//*tree = new Tree("TBODY", start, m_input - start);
		//(*tree)->m_children.push_back(child1);
		//(*tree)->m_children.push_back(child2);

		*tree = child2;

		return true;
	}

	return false;
}

bool SceneParser::ROWS(Tree** tree) {// ROWS: ROW*
	Tree* child = nullptr;
	std::list<Tree*> list;
	char* start = m_input;
	//bool comma = true;

	while (ROW(&child)) {
		list.push_back(child);
		
		if (!TERM(",\n", &child)) {
			//comma = false;
			break;
		}
	}

	if (!TERM("\n", &child)) {
		return false;
	}

	if (list.size() > 0) {
		*tree = new Tree("ROWS", start, m_input - start);
		(*tree)->m_children = list;
	}

	return true;
}

bool SceneParser::ROW(Tree** tree) {
	Tree* child = nullptr;
	std::list<Tree*> list;
	char* start = m_input;
	bool comma = true;

	WHITESPACE();

	while (VECTOR3(&child)) {
		list.push_back(child);

		if (!TERM(", ", &child)) {
			break;
		}
	}

	if (list.size() > 0) {
		*tree = new Tree("ROW", start, m_input - start);
		(*tree)->m_children = list;
	}

	return true;
}

bool SceneParser::ROWLAST(Tree** tree) {
	/*Tree* child = nullptr;
	char* start = m_input;

	while (VECTOR3(&child)) {
		list.push_back(child);

		if (!TERM(", ", &child)) {
			break;
		}
	}

	*/
	return false;
}

bool SceneParser::LUA(Tree** tree) {// LUA:	"Mesh(" STRING ")\n" LBODY
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	Tree* child3 = nullptr;
	Tree* child4 = nullptr;
	char* start = m_input;

	if (TERM("Mesh(", &child1) && STRING(&child2) && TERM(")\n", &child3) && LBODY(&child4)) {
		*tree = new Tree("LUA", start, m_input - start);
		//(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);
		//(*tree)->m_children.push_back(child3);
		(*tree)->m_children.push_back(child4);

		return true;
	}

	return false;
}

bool SceneParser::LBODY(Tree** tree) {// LBODY: "Lua(<\n" CODE ">)\n"
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	Tree* child3 = nullptr;
	Tree* child4 = nullptr;
	char* start = m_input;

	if (TERM("Lua(<\n", &child1) && CODE(&child2) && TERM(">)\n", &child3)) {
		//*tree = new Tree("LUA", start, m_input - start);
		
		//(*tree)->m_children.push_back(child1);
		//(*tree)->m_children.push_back(child2);
		//(*tree)->m_children.push_back(child3);
		//(*tree)->m_children.push_back(child4);

		*tree = child2;

		return true;
	}

	return false;
}

bool SceneParser::CODE(Tree** tree) {// CODE: [^">)"]*
	Tree* child;
	char* start = m_input;
	//bool running = true;

	while (!TERM(">)", &child)) {
		m_input++;
	}
	m_input -= 2;
	*tree = new Tree("CODE", start, m_input - start);

	return true;
}

bool SceneParser::WHITESPACE() {// WHITESPACE:	['\t'' ''\n']*
	CharClass ws("\n\t ");
	Star s(&ws);

	int nr = s.match(m_input);
	m_input += nr;

	return true;
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
		//*tree = new Tree("MBODY", start, m_input - start);
		//(*tree)->m_children.push_back(child1);
		//(*tree)->m_children.push_back(child2);
		//(*tree)->m_children.push_back(child3);

		*tree = child2;

		return true;
	}

	return false;
}

bool SceneParser::TRIANGLES(Tree** tree) {// TRIANGLES:	TRIANGLE* | TRIANGLE5*
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

	while (TRIANGLE5(&child)) {
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

	if (WHITESPACE() && VECTOR3(&child1) && TERM(",\n", &child2) && WHITESPACE() && VECTOR3(&child3) && TERM(",\n", &child4) && WHITESPACE() && VECTOR3(&child5)) {
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

bool SceneParser::TRIANGLE5(Tree** tree) {//VECTOR5 ",\n" VECTOR5 ",\n" VECTOR5
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	Tree* child3 = nullptr;
	Tree* child4 = nullptr;
	Tree* child5 = nullptr;
	char* start = m_input;

	if (WHITESPACE() && VECTOR5(&child1) && TERM(",", &child2) && WHITESPACE() && VECTOR5(&child3) && TERM(",", &child4) && WHITESPACE() && VECTOR5(&child5)) {
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
	m_input = start; //do we need this i think important yes very important
	return false;
}

bool SceneParser::VECTOR5(Tree** tree) {//"(" NUMBER "," NUMBER "," NUMBER "," NUMBER "," NUMBER")"
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	Tree* child3 = nullptr;
	Tree* child4 = nullptr;
	Tree* child5 = nullptr;
	Tree* child6 = nullptr;
	Tree* child7 = nullptr;
	Tree* child8 = nullptr;
	Tree* child9 = nullptr;
	char* start = m_input;

	if (TERM("(", &child1) && NUMBER(&child2) && TERM(",", &child3) && NUMBER(&child4) && TERM(",", &child5) && NUMBER(&child6) && TERM(",", &child5) && NUMBER(&child7) && TERM(",", &child5) && NUMBER(&child8) && TERM(")", &child9)) {
		*tree = new Tree("VECTOR3", start, m_input - start);
		//(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);
		//(*tree)->m_children.push_back(child3);
		(*tree)->m_children.push_back(child4);
		//(*tree)->m_children.push_back(child5);
		(*tree)->m_children.push_back(child6);
		//(*tree)->m_children.push_back(child7);
		(*tree)->m_children.push_back(child7);
		(*tree)->m_children.push_back(child8);

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


bool SceneParser::SBODY(Tree** tree) {// SBODY:	"\n{\n" SFUNCTIONS* "}"
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	Tree* child3 = nullptr;
	char *start = m_input;

	if (TERM("\n{\n", &child1)) {
		std::list<Tree*> list;

		while (SFUNCTIONS(&child2)) {
			list.push_back(child2);
		//(*tree)->m_children.push_back(child1);
		//(*tree)->m_children.push_back(child2);
		//(*tree)->m_children.push_back(child3);
		}

		if (TERM("}", &child3)) {

			*tree = new Tree("SBODY", start, m_input - start);
			(*tree)->m_children = list;
			return true;
		}

		return false;
	}
	return false;
}

bool SceneParser::SFUNCTIONS(Tree** tree) {// SFUNCTIONS: SMESH | BIND
	Tree *child1 = nullptr, *child2 = nullptr, *child3 = nullptr, *child4 = nullptr;
	char* start = m_input;
	
	WHITESPACE();

	if (TERM("Mesh", &child1) && TERM("(", &child4) && STRING(&child2) && TERM(")\n", &child3)) {// SMESH: "Mesh" "(" STRING ")\n"
		*tree = new Tree("Mesh", start, m_input - start);
		if (child1)
			//(*tree)->m_children.push_back(child1);
		if (child2)
			(*tree)->m_children.push_back(child2);
		if (child3)
			//(*tree)->m_children.push_back(child3);
			if (child4);
			//(*tree)->m_children.push_back(child4);

		return true;
	}
	else if (TERM("Bind", &child1) && TERM("(", &child2) && STRING(&child3) && TERM(", ", &child2) && TERM("Mesh", &child4) && TERM("(", &child4) && STRING(&child2) && TERM("))\n", &child4) ) {// BIND: "Bind" "(" STRING ", " SMESH ")\n"
		*tree = new Tree("Bind", start, m_input - start);

		
		(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child3);
		(*tree)->m_children.push_back(child2);

		return true;
	}

	return false;
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
		//*tree = new Tree("STRING", start, m_input - start);
		//(*tree)->m_children.push_back(child1);
		//(*tree)->m_children.push_back(child2);
		//(*tree)->m_children.push_back(child3);

		*tree = child2;

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