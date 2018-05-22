#include "SceneParser.h"
#include <iostream>

// FILE:		FUNCTION* SCENE
// FUNCTION:	MESH | LUA | TEXTURE 
// MESH:		"Mesh(" STRING ")" MBODY
// MBODY:		WHITESPACE "{\n" TRIANGLES WHITESPACE "}\n"
// TRIANGLES:	TRIANGLE* | TRIANGLE5*
// TRIANGLE:	VECTOR3 ",\n" VECTOR3 ",\n" VECTOR3
// TRIANGLE5:	VECTOR5 ",\n" VECTOR5 ",\n" VECTOR5
// VECTOR3:		"(" NUMBER "," NUMBER "," NUMBER ")"
// VECTOR5:		"(" NUMBER "," NUMBER "," NUMBER "," NUMBER "," NUMBER")"
// NUMBER:		"-" [0-9]* | [0-9]*
// SCENE:		"Scene()" SBODY
// SBODY:		WHITESPACE "{" WHITESPACE SFUNCTIONS* WHITESPACE "}"
// SFUNCTIONS:	WHITESPACE (SMESH | BIND WHITESPACE ")")
// SMESH:		"Mesh" "(" STRING ")" WHITESPACE
// BIND:		"Bind" "(" WHITESPACE STRING ","  WHITESPACE (SMESH | TRANSFORM)
// STRING:		'\"' [a-z]+ '\"'
// TEXTURE:		"Texture(" STRING ")\n" TBODY
// TBODY:		WHITESPACE "{" WHITESPACE ROWS WHITESPACE "}\n" | WHITESPACE LBODY
// ROWS:		ROW*
// ROW:			WHITESPACE VECTOR3*
// LUA:			"Mesh(" STRING ")\n" LBODY "\n"
// LBODY:		"Lua(<\n" CODE ">)"
// CODE:		[^">)"]*
// TRANSFORM:	WHITESPACE "Transform(" WHITESPACE LBODY "," WHITESPACE SMESH WHITESPACE ")"
// WHITESPACE:	['\t'' ''\n']*


SceneParser::SceneParser(char* input) : m_input(input), digit("0123456789"), hex("0123456789abcdefABCDEF"), nonzero("123456789"), dot("."),
letters("QWERTYUIOPASDFGHJKLMNBVCXZqwertyuiopasdfghjklzxcvbnm"), sDigit(&digit), number({ &nonzero, &sDigit }), zero("0"), x("x"), sHex(&hex),
hexnumber({ &zero, &x, &hex, &sHex }), sLetters(&letters), word({ &letters, &sLetters, &sDigit }) {
	FILE(&m_root);
}


SceneParser::~SceneParser() {
	delete m_root;
}

void SceneParser::buildScene(lua_State* L) {
	if (m_root == nullptr)
		return;


	Tree* tree = m_root->m_children.back();

	if (tree->m_tag != "SCENE") {
		std::cout << "no scene function" << std::endl;
		return;
	}

	buildTexture(L);
	tree = tree->m_children.back();

	std::string name;
	for (auto p : tree->m_children) {
		
		if (p->m_tag == "Mesh") {
			buildMesh(L, p->m_children.front()->m_lexeme);
		}
		else if (p->m_tag == "Bind") {
			name = p->m_children.front()->m_lexeme;
			buildMesh(L, name);
			lua_getglobal(L, "bind");
			lua_pushstring(L, name.c_str());
			
			lua_pushstring(L, p->m_children.back()->m_lexeme.c_str());
			lua_pcall(L, 2, 0, 0);
		}
		else if (p->m_tag == "Transform") {
			std::list<Tree*>::iterator it = p->m_children.begin();
			it++;
			name = (*it)->m_lexeme;
			buildMesh(L, name, p->m_children.front()->m_lexeme.c_str());//name  lua kod

			lua_getglobal(L, "bind");
			lua_pushstring(L, name.c_str());
			
			lua_pushstring(L, p->m_children.back()->m_lexeme.c_str());
			lua_pcall(L, 2, 0, 0);
		}
	}
}

void SceneParser::buildMesh(lua_State* L, std::string& arg, const char *p) {
	Tree* tree = nullptr;
	bool code = false;

	for (auto p : m_root->m_children) {
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

	
	if (code) {
		lua_getglobal(L, "addMesh");
		
		if (luaL_loadstring(L, "local args = {...} tri = {} for k, v in pairs(args[1]) do tri[k] = {args[2](v[1],v[2],v[3],v[4],v[5])} end return tri")) {//2
			std::cout << lua_tostring(L, -1) << '\n';
			lua_pop(L, 1);
		}
		
		// create mesh
		if (luaL_loadstring(L, tree->m_lexeme.c_str()) || lua_pcall(L, 0, 1, 0)) {
			std::cout << lua_tostring(L, -1) << '\n';
			lua_pop(L, 1);
		}
		if (p) {
			if (luaL_loadstring(L, p)) {
				std::cout << lua_tostring(L, -1) << '\n';
				lua_pop(L, 1);
			}
			arg += std::string(std::to_string(m_counter));
			m_counter++;
		}
		else {
			if (luaL_loadstring(L, "local args = {...} return args[1], args[2], args[3], args[4], args[5]")) {
				std::cout << lua_tostring(L, -1) << '\n';
				lua_pop(L, 1);
			}
		}
		if (lua_pcall(L, 2, 1, 0)) {
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
	lua_getglobal(L, "addMesh");
	lua_newtable(L);
	int index1 = 1;
	for (auto p : tree->m_children) {
		for (auto vector : p->m_children) {
			lua_newtable(L);
			int index2 = 1;
			for (auto number : vector->m_children) {
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
}

void SceneParser::buildTexture(lua_State* L) {
	for (auto p : m_root->m_children) {
		if (p->m_tag == "TEXTURE") {
			addTexture(L, p);
		}
	}
}

void SceneParser::addTexture(lua_State* L, Tree* tree) {
	std::string name = tree->m_children.front()->m_lexeme;
	tree = tree->m_children.back(); //rows or code

	if (tree->m_tag == "CODE") {
		lua_getglobal(L, "addTexture");

		// create texture
		if (luaL_loadstring(L, tree->m_lexeme.c_str()) || lua_pcall(L, 0, 1, 0)) {
			std::cout << lua_tostring(L, -1) << '\n';
			lua_pop(L, 1);
		}

		lua_pushstring(L, name.c_str());

		if (lua_pcall(L, 2, 0, 0)) {
			std::cout << lua_tostring(L, -1) << '\n';
			lua_pop(L, 1);
		}

		return;
	}
	

	lua_getglobal(L, "addTexture");
	lua_newtable(L);
	int rowNr = 1;
	for (auto row : tree->m_children) {
		lua_newtable(L);
		int vectorNr = 1;
		for (auto vector : row->m_children) {
			lua_newtable(L);
			int index = 1;
			for (auto number : vector->m_children) {
				lua_pushnumber(L, std::stoi(number->m_lexeme));
				lua_rawseti(L, -2, index);
				index++;
			}
			lua_rawseti(L, -2, vectorNr);
			vectorNr++;
		}
		lua_rawseti(L, -2, rowNr);
		rowNr++;
	}
	lua_pushstring(L, name.c_str());
	lua_pcall(L, 2, 0, 0);
}

bool SceneParser::FILE(Tree** tree) {// FILE: FUNCTION* SCENE
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

		for (auto l : list)
			delete l;

		return false;
	}

	return true;
}

bool SceneParser::FUNCTION(Tree** tree) {// FUNCTION: MESH | LUA | TEXTURE 
	Tree *child = nullptr;
	char* start = m_input;
	if (MESH(&child) || TEXTURE(&child) || LUA(&child)) {
		*tree = child;

		return true;
	}
	return false;
}

bool SceneParser::MESH(Tree** tree) {// MESH: "Mesh(" STRING ")" MBODY
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	char* start = m_input;

	if (TERM("Mesh(") && STRING(&child1) && TERM(")") && MBODY(&child2)) {
		*tree = new Tree("MESH", start, m_input - start);
		(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);

		return true;
	}
	m_input = start;

	delete child1;

	return false;
}

bool SceneParser::TEXTURE(Tree** tree) {// TEXTURE: "Texture(" STRING ")\n" TBODY
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	
	char* start = m_input;

	if (TERM("Texture(") && STRING(&child1) && TERM(")\n") && TBODY(&child2)) {
		*tree = new Tree("TEXTURE", start, m_input - start);
		(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);

		return true;
	}

	delete child1;

	return false;
}

bool SceneParser::TBODY(Tree** tree) {// TBODY:	WHITESPACE "{" WHITESPACE ROWS WHITESPACE "}\n" | WHITESPACE LBODY
	Tree* child = nullptr;
	char* start = m_input;

	if (WHITESPACE() && TERM("{") && WHITESPACE() && ROWS(&child) && WHITESPACE() && TERM("}\n")) {
		*tree = child;

		return true;
	}
	else if (WHITESPACE() && LBODY(&child)) {
		*tree = child;

		return true;
	}

	return false;
}

bool SceneParser::ROWS(Tree** tree) {// ROWS: ROW*
	Tree* child = nullptr;
	std::list<Tree*> list;
	char* start = m_input;

	while (ROW(&child)) {
		list.push_back(child);
		
		if (!TERM(",\n")) {
			break;
		}
	}

	if (!TERM("\n")) {
		for (auto p : list)
			delete p;

		return false;
	}

	*tree = new Tree("ROWS", start, m_input - start);
	(*tree)->m_children = list;

	return true;
}

bool SceneParser::ROW(Tree** tree) {// ROW: WHITESPACE VECTOR3*
	Tree* child = nullptr;
	std::list<Tree*> list;
	char* start = m_input;
	bool comma = true;

	WHITESPACE();

	while (VECTOR3(&child)) {
		list.push_back(child);

		if (!TERM(", ")) {
			break;
		}
	}

	
	*tree = new Tree("ROW", start, m_input - start);
	(*tree)->m_children = list;

	return true;
}

bool SceneParser::LUA(Tree** tree) {// LUA: "Mesh(" STRING ")\n" LBODY "\n"
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	char* start = m_input;

	if (TERM("Mesh(") && STRING(&child1) && TERM(")\n") && LBODY(&child2) && TERM("\n")) {
		*tree = new Tree("LUA", start, m_input - start);
		(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);

		return true;
	}

	delete child1;

	return false;
}

bool SceneParser::LBODY(Tree** tree) {// LBODY: "Lua(<\n" CODE ">)"
	Tree* child = nullptr;
	char* start = m_input;

	if (WHITESPACE() && TERM("Lua(<") && CODE(&child) && TERM(">)")) {
		*tree = child;

		return true;
	}

	return false;
}

bool SceneParser::CODE(Tree** tree) {// CODE: [^">)"]*
	Tree* child;
	char* start = m_input;

	while (!TERM(">)")) {
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

bool SceneParser::TRANSFORM(Tree** tree) {//TRANSFORM: WHITESPACE "Transform(" WHITESPACE LBODY "," WHITESPACE SMESH WHITESPACE ")"
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	char* start = m_input;

	WHITESPACE();

	if (TERM("Transform(") && WHITESPACE() && LBODY(&child1) && TERM(",") && WHITESPACE() && SMESH(&child2) && WHITESPACE() && TERM(")")) {
		*tree = new Tree("Transform", start, m_input - start);
		(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);
		return true;
	}

	delete child1;

	return false;
}

bool SceneParser::SCENE(Tree** tree) {// SCENE:	"Scene()" SBODY
	Tree* child = nullptr;
	char* start = m_input;

	if (TERM("Scene()") && SBODY(&child)) {
		*tree = new Tree("SCENE", start, m_input - start);
		(*tree)->m_children.push_back(child);

		return true;
	}

	return false;
}

bool SceneParser::MBODY(Tree** tree) {// MBODY:	WHITESPACE "{\n" TRIANGLES WHITESPACE "}\n"
	Tree* child = nullptr;
	char* start = m_input;

	if (WHITESPACE() && TERM("{\n") && TRIANGLES(&child) && WHITESPACE() && TERM("}\n")) {
		*tree = child;

		return true;
	}

	delete child;

	return false;
}

bool SceneParser::TRIANGLES(Tree** tree) {// TRIANGLES:	TRIANGLE* | TRIANGLE5*
	Tree* child = nullptr;
	std::list<Tree*> list;
	char* start = m_input;
	bool comma = true;

	while (TRIANGLE(&child)) {
		list.push_back(child);

		if (!TERM(",\n")) {
			comma = false;
			break;
		}
	}

	while (TRIANGLE5(&child)) {
		list.push_back(child);

		if (!TERM(",\n")) {
			comma = false;
			break;
		}
	}

	if (!comma && !TERM("\n")) {
		for (auto p : list)
			delete p;

		return false;
	}

	
	*tree = new Tree("TRIANGLES", start, m_input - start);
	(*tree)->m_children = list;
	

	return true;
}

bool SceneParser::TRIANGLE(Tree** tree) {// TRIANGLE:	VECTOR3 ",\n" VECTOR3 ",\n" VECTOR3
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	Tree* child3 = nullptr;
	char* start = m_input;

	if (WHITESPACE() && VECTOR3(&child1) && TERM(",\n") && WHITESPACE() && VECTOR3(&child2) && TERM(",\n") && WHITESPACE() && VECTOR3(&child3)) {
		*tree = new Tree("TRIANGLE", start, m_input - start);
		(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);
		(*tree)->m_children.push_back(child3);

		return true;
	}

	m_input = start;

	delete child1;
	delete child2;
	delete child3;

	return false;
}

bool SceneParser::TRIANGLE5(Tree** tree) {//VECTOR5 ",\n" VECTOR5 ",\n" VECTOR5
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	Tree* child3 = nullptr;
	char* start = m_input;

	if (WHITESPACE() && VECTOR5(&child1) && TERM(",") && WHITESPACE() && VECTOR5(&child2) && TERM(",") && WHITESPACE() && VECTOR5(&child3)) {
		*tree = new Tree("TRIANGLE", start, m_input - start);
		(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);
		(*tree)->m_children.push_back(child3);

		return true;
	}

	m_input = start;

	delete child1;
	delete child2;
	delete child3;

	return false;
}

bool SceneParser::VECTOR3(Tree** tree) {//"(" NUMBER "," NUMBER "," NUMBER ")"
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	Tree* child3 = nullptr;
	char* start = m_input;
	if (TERM("(") && NUMBER(&child1) && TERM(",") && NUMBER(&child2) && TERM(",") && NUMBER(&child3) && TERM(")")) {
		*tree = new Tree("VECTOR3", start, m_input - start);
		(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);
		(*tree)->m_children.push_back(child3);

		return true;
	}

	m_input = start;

	delete child1;
	delete child2;
	delete child3;


	return false;
}

bool SceneParser::VECTOR5(Tree** tree) {//"(" NUMBER "," NUMBER "," NUMBER "," NUMBER "," NUMBER")"
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	Tree* child3 = nullptr;
	Tree* child4 = nullptr;
	Tree* child5 = nullptr;
	char* start = m_input;

	if (TERM("(") && NUMBER(&child1) && TERM(",") && NUMBER(&child2) && TERM(",") && NUMBER(&child3) && TERM(",") && NUMBER(&child4) && TERM(",") && NUMBER(&child5) && TERM(")")) {
		*tree = new Tree("VECTOR3", start, m_input - start);
		(*tree)->m_children.push_back(child1);
		(*tree)->m_children.push_back(child2);
		(*tree)->m_children.push_back(child3);
		(*tree)->m_children.push_back(child4);
		(*tree)->m_children.push_back(child5);

		return true;
	}

	m_input = start;

	delete child1;
	delete child2;
	delete child3;
	delete child4;
	delete child5;

	return false;
}

bool SceneParser::NUMBER(Tree** tree) {//"-"[0 - 9] * | [0 - 9] *
	char *start = m_input;
	Tree *child = nullptr;

	if (TERM("-")) {
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


bool SceneParser::SBODY(Tree** tree) {// SBODY:	WHITESPACE "{" WHITESPACE SFUNCTIONS* WHITESPACE "}"
	Tree* child = nullptr;
	char *start = m_input;
	std::list<Tree*> list;

	if (WHITESPACE() && TERM("{") && WHITESPACE()) {

		while (SFUNCTIONS(&child)) {
			list.push_back(child);
		}

		if (WHITESPACE() && TERM("}")) {

			*tree = new Tree("SBODY", start, m_input - start);
			(*tree)->m_children = list;
			return true;
		}
	}

	for (auto p : list)
		delete p;

	return false;
}

bool SceneParser::SFUNCTIONS(Tree** tree) {// SFUNCTIONS: WHITESPACE (SMESH | BIND WHITESPACE ")")
	Tree *child = nullptr;
	char* start = m_input;
	
	WHITESPACE();
	if (SMESH(&child)) {
		*tree = new Tree("Mesh", start, m_input - start);
		(*tree)->m_children.push_back(child);

		return true;
	}
	else if (BIND(&child) && WHITESPACE() && TERM(")")) {
		*tree = child;

		return true;
	}

	return false;
}

bool SceneParser::BIND(Tree** tree) {//BIND: "Bind" "(" WHITESPACE STRING ","  WHITESPACE (SMESH | TRANSFORM)
	Tree* child1 = nullptr;
	Tree* child2 = nullptr;
	char *start = m_input;

	if (TERM("Bind") && TERM("(") && WHITESPACE() && STRING(&child1) && TERM(",") && WHITESPACE()) {
		if (SMESH(&child2)) {
			*tree = new Tree("Bind", start, m_input - start);
			(*tree)->m_children.push_back(child2);
			(*tree)->m_children.push_back(child1);

			return true;
		}
		else if (TRANSFORM(&child2)) {
			*tree = child2;
			(*tree)->m_children.push_back(child1);

			return true;
		}
	}

	delete child1;
	delete child2;

	return false;
}

bool SceneParser::SMESH(Tree** tree = nullptr) {// SMESH: "Mesh" "(" STRING ")" WHITESPACE
	Tree *child = nullptr;
	char* start = m_input;

	if (TERM("Mesh") && TERM("(") && STRING(&child) && TERM(")") && WHITESPACE()) {
		*tree = child;

		return true;
	}

	delete child;

	return false;
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
	Tree* child = nullptr;
	char* start = m_input;

	if (TERM("\"") && WORD(&child) && TERM("\"")) {
		*tree = child;

		return true;
	}

	delete child;

	return false;
}

bool SceneParser::TERM(const char *lit)
{
	int i;
	char* start = m_input;
	for (i = 0; lit[i] != 0; i++)
		if (m_input[i] != lit[i])
			return false;
	m_input += i;

	return true;
}