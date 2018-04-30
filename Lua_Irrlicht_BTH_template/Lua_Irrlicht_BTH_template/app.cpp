#include "app.h"
#include <iostream>
#include <path.h>
#include <string>

irr::scene::ISceneManager* App::m_smgr = nullptr;
irr::video::IVideoDriver* App::m_driver = nullptr;
irr::gui::IGUIEnvironment* App::m_guienv = nullptr;
std::vector<irr::scene::IMeshSceneNode*> App::m_boxes;
int App::m_id = 0;


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

void App::drawOneFrame()
{
	m_driver->beginScene(true, true, irr::video::SColor(255, 90, 101, 140));

	m_smgr->drawAll();
	m_guienv->drawAll();

	m_driver->endScene();
}

App::App()
{
	m_device = irr::createDevice(irr::video::EDT_SOFTWARE, irr::core::dimension2d<irr::u32>(640, 480), 16, false, false, true, 0);

	if (!m_device) {
		//error
	}
	m_device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");
	m_driver = m_device->getVideoDriver();
	m_guienv = m_device->getGUIEnvironment();
	m_smgr = m_device->getSceneManager();

	m_guienv->addStaticText(L"Hello World! This is the Irrlicht Software renderer!", irr::core::rect<irr::s32>(10, 10, 260, 22), true);

	irr::SKeyMap keymap[4];
	keymap[0].Action = irr::EKA_MOVE_FORWARD;
	keymap[0].KeyCode = irr::KEY_KEY_W;
	keymap[1].Action = irr::EKA_MOVE_BACKWARD;
	keymap[1].KeyCode = irr::KEY_KEY_S;
	keymap[2].Action = irr::EKA_STRAFE_LEFT;
	keymap[2].KeyCode = irr::KEY_KEY_A;
	keymap[3].Action = irr::EKA_STRAFE_RIGHT;
	keymap[3].KeyCode = irr::KEY_KEY_D;

	m_smgr->addCameraSceneNodeFPS(0, 100, 0.5, -1, keymap, 4);

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
		//m_device->yield();
		m_smgr->getActiveCamera()->setInputReceiverEnabled(false);
	}
	drawOneFrame();
}

int App::addMesh(lua_State * L)
{
	int size = lua_gettop(L);

	if (size == 1 && lua_istable(L, -1)) {
		std::vector<irr::core::vector3df> list;
		int index = 1;
		while (lua_geti(L, -1, index) == LUA_TTABLE) {
			irr::core::vector3df vec;
			if (isVector(L, vec)) {
				list.push_back(vec);
			}
			else {
				lua_pop(L, 1);
				std::cout << "error in triangle list" << std::endl;
				return 0;
			}
			index++;
			lua_pop(L, 1);
		}
		std::cout << "added elements: " << index - 1 << std::endl;
		for (auto& vec : list) {
			std::cout << vec.X << ", " << vec.Y << ", " << vec.Z << std::endl;
		}
	}
	else {
		std::cout << "To wrong arguments" << std::endl;
	}


	return 0;
}

int App::addBox(lua_State * L)
{
	irr::core::vector3df ori;
	int size;
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
		m_boxes.operator[](m_boxes.size() - 1)->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		m_boxes.operator[](m_boxes.size() - 1)->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);
		std::cout << "succes" << std::endl;
		//drawOneFrame();
	}
	
	
	return 0;
}

int App::getNodes(lua_State * L)
{
	return 0;
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

lua_State * App::getLuaState()
{
	return this->L;
}
