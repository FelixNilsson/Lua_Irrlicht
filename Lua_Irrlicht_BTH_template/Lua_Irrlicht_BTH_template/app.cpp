#include "app.h"
#include "SceneParser.h"
#include <iostream>
#include <path.h>
#include <string>
#include <fstream>
#include <sstream>

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif 

using namespace irr;
using namespace scene;
using namespace core;
using namespace video;
using namespace gui;

irr::IrrlichtDevice* App::m_device = nullptr;
irr::scene::ISceneManager* App::m_smgr = nullptr;
irr::video::IVideoDriver* App::m_driver = nullptr;
irr::gui::IGUIEnvironment* App::m_guienv = nullptr;
std::vector<irr::scene::IMeshSceneNode*> App::m_meshes;
std::vector<irr::scene::IMeshSceneNode*> App::m_boxes;
int App::m_id = 0;

int App::test(lua_State* L) {
	int size = lua_gettop(L);

	std::cout << size << std::endl;
	lua_pushnumber(L, 11);
	size = lua_gettop(L);
	std::cout << size << std::endl;
	lua_getglobal(L, "test1");
	size = lua_gettop(L);
	std::cout << size << std::endl;
	lua_pushnumber(L, 1);
	size = lua_gettop(L);
	std::cout << size << std::endl;
	if (lua_pcall(L, 1, 3, 0)) {
		std::cout << lua_tostring(L, -1) << '\n';
		lua_pop(L, 1);
	}
	size = lua_gettop(L);
	std::cout << size << std::endl;
	

	return 0;
}

int App::test1(lua_State* L) {
	int size = lua_gettop(L);
	std::cout << size << std::endl;

	return 0;
}

bool App::isVector(lua_State * L, irr::core::vector3df &vector)
{
	//size_t len = lua_rawlen(L,1); ???
	int isNumber = 0;
	lua_pushnumber(L, 1);
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1)) {
		vector.X = lua_tonumber(L, -1);
		isNumber++;
	} 
	else {
		std::cout << "expected a number" << std::endl;
	}
	lua_pop(L, 1);
	lua_pushnumber(L, 2);
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1)) {
		vector.Y = lua_tonumber(L, -1);
		isNumber++;
	}
	else {
		std::cout << "expected a number" << std::endl;
	}
	lua_pop(L, 1);
	lua_pushnumber(L, 3);
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1)) {
		vector.Z = lua_tonumber(L, -1);
		isNumber++;
	}
	else {
		std::cout << "expected a number" << std::endl;
	}
	lua_pop(L, 1);
	lua_pushnumber(L, 4);
	if (lua_gettable(L, -2)) {
		isNumber++;
		std::cout << "to many arguments in the table" << std::endl;
	}
	lua_pop(L, 1);
	if (isNumber < 3)
		std::cout << "to few arguments in the table" << std::endl;
		
	return isNumber == 3;
}

bool App::isVectorUV(lua_State * L, vectorUV &vector)
{
	//size_t len = lua_rawlen(L,1); ???
	int isNumber = 0;
	lua_pushnumber(L, 1);
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1)) {
		vector.vec.X = lua_tonumber(L, -1);
		isNumber++;
	}
	else {
		std::cout << "expected a number" << std::endl;
	}
	lua_pop(L, 1);
	lua_pushnumber(L, 2);
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1)) {
		vector.vec.Y = lua_tonumber(L, -1);
		isNumber++;
	}
	else {
		std::cout << "expected a number" << std::endl;
	}
	lua_pop(L, 1);
	lua_pushnumber(L, 3);
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1)) {
		vector.vec.Z = lua_tonumber(L, -1);
		isNumber++;
	}
	else {
		std::cout << "expected a number" << std::endl;
	}
	lua_pop(L, 1);
	lua_pushnumber(L, 4);
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1)) {
		vector.uv.X = lua_tonumber(L, -1);
		isNumber++;
	}
	else {
		std::cout << "expected a number" << std::endl;
	}
	lua_pop(L, 1);
	lua_pushnumber(L, 5);
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1)) {
		vector.uv.Y = lua_tonumber(L, -1);
		isNumber++;
	}
	else {
		std::cout << "expected a number" << std::endl;
	}


	lua_pop(L, 1);
	lua_pushnumber(L, 6);
	if (lua_gettable(L, -2)) {
		isNumber++;
		std::cout << "to many arguments in the table" << std::endl;
	}
	lua_pop(L, 1);
	if (isNumber < 5)
		std::cout << "to few arguments in the table" << std::endl;

	return isNumber == 5;
}

void App::drawOneFrame()
{
	m_driver->beginScene(true, true, irr::video::SColor(255, 90, 101, 140));

	m_smgr->drawAll();
	m_guienv->drawAll();

	m_driver->endScene();
}

bool App::isPowerOfTwo(int nr)
{
	int temp = 2;
	while (nr > temp) {
		temp *= 2;
	}
	return nr == temp;
}

irr::scene::ISceneNode* App::getSceneNode(std::string nodeName)
{
	const irr::scene::ISceneNodeList& list = m_smgr->getRootSceneNode()->getChildren();
	irr::scene::ISceneNode *node = nullptr;
	for (auto e : list) {
		if (e->getName() == nodeName) {
			node = e;
		}
			
	}
	return node;
}

void App::destroyScene()
{
	const irr::scene::ISceneNodeList& list = m_smgr->getRootSceneNode()->getChildren();
	for (auto e : list) {
		auto s = e->getType();
		if (s != ESNT_CAMERA)
		e->remove();

	}
}

App::App()
{
	std::cout << new int << std::endl;
	m_device = irr::createDevice(irr::video::EDT_SOFTWARE, irr::core::dimension2d<irr::u32>(640, 480), 16, false, false, true, 0);

	if (!m_device) {
		//error
	}
	m_device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");
	m_driver = m_device->getVideoDriver();
	m_guienv = m_device->getGUIEnvironment();
	m_smgr = m_device->getSceneManager();

	m_guienv->addStaticText(L"Hello World! This is the Irrlicht Software renderer!", irr::core::rect<irr::s32>(10, 10, 260, 22), true);

	irr::SKeyMap keymap[5];
	keymap[0].Action = irr::EKA_MOVE_FORWARD;
	keymap[0].KeyCode = irr::KEY_KEY_W;
	keymap[1].Action = irr::EKA_MOVE_BACKWARD;
	keymap[1].KeyCode = irr::KEY_KEY_S;
	keymap[2].Action = irr::EKA_STRAFE_LEFT;
	keymap[2].KeyCode = irr::KEY_KEY_A;
	keymap[3].Action = irr::EKA_STRAFE_RIGHT;
	keymap[3].KeyCode = irr::KEY_KEY_D;
	keymap[4].Action = irr::EKA_JUMP_UP;
	keymap[4].KeyCode = irr::KEY_SPACE;

	m_smgr->addCameraSceneNodeFPS(0, 100, 0.1, -1, keymap, 5, false, 100, false, true);

	this->L = luaL_newstate();
	luaL_openlibs(this->L);

	lua_pushcfunction(this->L, camera);
	lua_setglobal(this->L, "camera");
	lua_pushcfunction(this->L, addMesh);
	lua_setglobal(this->L, "addMesh");
	lua_pushcfunction(this->L, addBox);
	lua_setglobal(this->L, "addBox");
	lua_pushcfunction(this->L, getNodes);
	lua_setglobal(this->L, "getNodes");
	lua_pushcfunction(this->L, snapshot);
	lua_setglobal(this->L, "snapshot");
	lua_pushcfunction(this->L, addTexture);
	lua_setglobal(this->L, "addTexture");
	lua_pushcfunction(this->L, bind);
	lua_setglobal(this->L, "bind");
	lua_pushcfunction(this->L, loadScene);
	lua_setglobal(this->L, "loadScene");
	lua_pushcfunction(this->L, test);
	lua_setglobal(this->L, "test");
	lua_pushcfunction(this->L, test1);
	lua_setglobal(this->L, "test1");

	setupScript();
	
	m_smgr->getActiveCamera()->setPosition(irr::core::vector3df(0, 0, 15));
	m_smgr->getActiveCamera()->setTarget(irr::core::vector3df(0, 0, 0));
}

App::~App()
{
	m_device->drop();
}

bool App::run()
{
	return m_device->run();
}

void App::draw()
{

	if (m_device->isWindowActive()) {
		m_smgr->getActiveCamera()->setInputReceiverEnabled(true);
		
	}
	else {
		m_smgr->getActiveCamera()->setInputReceiverEnabled(false);
	}
	drawOneFrame();
}

int App::addMesh(lua_State * L)
{
	int size = lua_gettop(L);
	std::string name;

	if (size < 1 || size > 2) {
		std::cout << "To wrong arguments" << std::endl;
		return 0;
	}
	else if (size == 1) {
		if (!lua_istable(L, -1)) {
			std::cout << "expects a table" << std::endl;
			return 0;
		}
		name = "mesh" + std::to_string(m_id);
	}
	else if (size == 2) {
		if (!lua_istable(L, -2) && lua_type(L, -1) != LUA_TSTRING) {
			std::cout << "expects a table and a string" << std::endl;
			return 0;
		}
		name = lua_tostring(L, -1);
		lua_pop(L, 1);
	}
	
	std::vector<irr::core::vector3df> list;
	std::vector<vectorUV> listUV;
	irr::scene::SMesh* mesh = new SMesh();
	SMeshBuffer* buffer = new SMeshBuffer();
	mesh->addMeshBuffer(buffer);
	buffer->drop();
	//int index = 1;
	int type;
	lua_len(L, -1);
	int length = lua_tointeger(L, -1);
	lua_pop(L, 1);
	std::cout << length << std::endl;
	bool error = false;
	for (int i = 0; i < length && !error; i++) /*((type = lua_geti(L, -1, index)) == LUA_TTABLE)*/ {
		irr::core::vector3df vec;
		vectorUV vecUV;
		lua_geti(L, -1, i + 1);
		if (isVector(L, vec)) {
			list.push_back(vec);
		}
		else if (isVectorUV(L, vecUV)) {
			listUV.push_back(vecUV);
		}
		else {
			error = true;
		}
		lua_pop(L, 1);
	}
	if (error) {
		std::cout << "error in triangle list" << std::endl;
		std::cout << "WARNING!!! bad argument(s)" << std::endl;
	}
	else {
		m_id++;
		auto size = list.size();
		bool isUV = false;
		if (listUV.size() > 0) {
			size = listUV.size();
			isUV = true;
		}

		buffer->Vertices.reallocate(size);
		buffer->Vertices.set_used(size);
		buffer->Indices.reallocate(size);
		buffer->Indices.set_used(size);

		for (int i = 0; i < size; i++) {
			S3DVertex& v = buffer->Vertices[i];
			if (isUV) {
				v.TCoords.set(listUV[i].uv);
				v.Pos.set(listUV[i].vec);
			}
			else {
				v.Pos.set(list[i]);
			}
			buffer->Indices[i] = i;
		}		
		buffer->recalculateBoundingBox();
		auto p = m_smgr->addMeshSceneNode(mesh);
		p->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);
		p->setName(irr::core::string<char*>(name.c_str()));
		m_meshes.push_back(p);
	}
	//std::cout << "added elements: " << index - 1 << std::endl;
	for (auto& vec : list) {
		std::cout << vec.X << ", " << vec.Y << ", " << vec.Z << std::endl;
	}

	return 0;
}

int App::addBox(lua_State * L)
{
	irr::core::vector3df ori;
	float size;
	std::string name;
	bool correct = false;

	if (lua_type(L, -1) == LUA_TSTRING) {
		name = lua_tostring(L, -1);
		lua_pop(L, 1);
		if (lua_isnumber(L, -1)) {
			size = lua_tonumber(L, -1);
			lua_pop(L, 1);
			if (lua_istable(L, -1)) {
				correct = isVector(L, ori);
			}
		}
	}

	else {
		name = "box" + std::to_string(m_id);
		if (lua_isnumber(L, -1)) {
			size = lua_tonumber(L, -1);
			lua_pop(L, 1);
			if (lua_istable(L, -1)) {
				correct = isVector(L, ori);
			}
		}
	}
	
	if (correct) {
		m_boxes.push_back(m_smgr->addCubeSceneNode(size, 0, m_id, ori, irr::core::vector3df(0, 0, 0), irr::core::vector3df(1, 1, 1)));
		m_id++;
		m_boxes[m_boxes.size() - 1]->setName(irr::core::string<char*>(name.c_str()));
		m_boxes[m_boxes.size() - 1]->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		m_boxes[m_boxes.size() - 1]->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);
		std::cout << "succes" << std::endl;
	}
	
	
	return 0;
}

int App::getNodes(lua_State * L)
{
	lua_newtable(L);
	const irr::scene::ISceneNodeList& list = m_smgr->getRootSceneNode()->getChildren();

	int index = 1;
	for (auto e : list) {
		lua_newtable(L);
		lua_pushstring(L, e->getName());
		lua_rawseti(L, -2, 1);
		lua_pushinteger(L, e->getID());
		lua_rawseti(L, -2, 2);
		lua_rawseti(L, -2, index);
		index++;
	}

	return 1;
}

int App::camera(lua_State * L)
{
	int debug = lua_gettop(L);
	bool isPos = false;
	bool isTarget = false;
	irr::core::vector3df pos;
	irr::core::vector3df target;
	if (lua_istable(L, -1)) {
		isTarget = isVector(L, target);
		lua_pop(L, 1);

		if (lua_istable(L, -1)) {
			isPos = isVector(L, pos);
		}
		lua_pop(L, 1);
		if (isPos && isTarget) {
			m_smgr->getActiveCamera()->setPosition(pos);
			m_smgr->getActiveCamera()->setTarget(target);
			std::cout << "succes" << std::endl;
		}
	}
	return 0;
}

int App::snapshot(lua_State * L)
{
	irr::core::vector3df temp;
	temp = m_smgr->getActiveCamera()->getPosition();
	temp = m_smgr->getActiveCamera()->getAbsolutePosition();
	temp = m_smgr->getActiveCamera()->getTarget();
	if (lua_isstring(L, -1)) {
		std::string name = lua_tostring(L, -1);
		irr::video::IImage *screenshot = m_driver->createScreenShot();
		irr::io::path path(irr::core::string<char*>(name.c_str()));
	
		bool worked = m_driver->writeImageToFile(screenshot, path);
		std::cout << worked << std::endl;
	}
	
	return 0;
}

int App::addTexture(lua_State * L)
{
	int isTable;
	std::vector<irr::core::vector3df> color;
	//std::vector<irr::core::vector3df> row;
	bool error = false;
	int rowNr;
	int counter = 0;
	if (lua_isstring(L, -1)) {
		std::string name = lua_tostring(L, -1);
		lua_pop(L, 1);
		if (lua_istable(L, -1)) {//first table
			int Nr = 1;
			lua_pushnumber(L, Nr);
			while (isTable = lua_gettable(L, -2) && !error) {//second table
					int index = 1;
					lua_pushnumber(L, index);
					while (isTable = lua_gettable(L, -2) && !error) {//third table
						irr::core::vector3df temp;
						int debug = lua_istable(L, -1);
						if (isVector(L, temp))
							color.push_back(temp);
						else
							error = true;
						index++;
						lua_pop(L, 1);
						lua_pushnumber(L, index);

					}
					if (counter == 0)
						counter = index;
					else if (index != counter) {
						//wrong
						std::cout << "texture have wrong dimensions" << std::endl;
						return 0;
					}
					rowNr = index;
					Nr++;
					lua_pop(L, 2);
					lua_pushnumber(L, Nr);
			}
			Nr--;
			rowNr--;
			if (Nr == rowNr && isPowerOfTwo(Nr)) {
				char *data = new char[Nr * rowNr * 3];
				for (int i = 0; i < Nr; i++) {
					for (int k = 0; k < rowNr; k++) {
						data[i * rowNr * 3 + k * 3 + 0] = color[i * rowNr + k].X * 255;
						data[i * rowNr * 3 + k * 3 + 1] = color[i * rowNr + k].Y * 255;
						data[i * rowNr * 3 + k * 3 + 2] = color[i * rowNr + k].Z * 255;
					}
				}
				irr::video::IImage *p = m_driver->createImageFromData(ECOLOR_FORMAT::ECF_R8G8B8, dimension2du(Nr,rowNr), (void*)data, false, false);
				
				auto debugP = m_driver->addTexture(irr::core::string<char*>(name.c_str()), p, 0);
				if (debugP)
					std::cout << "texture works" << std::endl;
			}
		}
	}

	

	return 0;
}

int App::bind(lua_State * L)
{
	std::string textureName;
	std::string nodeName;
	if (lua_isstring(L, -1)) {
		textureName = lua_tostring(L, -1);
		lua_pop(L, 1);
		if (lua_isstring(L, -1)) {
			nodeName = lua_tostring(L, -1);
			auto node = getSceneNode(nodeName);
			if (node) {
				auto texture = m_driver->getTexture(irr::core::string<char*>(textureName.c_str()));
				if (texture) {
					node->setMaterialTexture(0, texture);
				}
			}
		}
	}
	return 0;
}

int App::loadScene(lua_State* L) {
	if (!lua_isstring(L, -1)) {
		std::cout << "wrong argument" << std::endl;
		return 0;
	}

	std::string filename = lua_tostring(L, -1);
	std::ifstream t(filename);
	if (!t.is_open()) {
		std::cout << "couldnt open file" << std::endl;
		return 0;
	}
	std::stringstream buffer;
	buffer << t.rdbuf();
	t.close();

	SceneParser parser(&buffer.str()[0]);

	destroyScene();
	parser.buildScene(L);

	return 0;
}

lua_State * App::getLuaState()
{
	return this->L;
}

void App::setupScript() {
	if (luaL_dofile(L, "setup3.lua")) {
		std::cout << "Couldn't load script" << std::endl;
		return;
	}
}