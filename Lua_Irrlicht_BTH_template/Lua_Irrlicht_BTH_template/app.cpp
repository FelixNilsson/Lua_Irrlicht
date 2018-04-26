#include "app.h"
#include <iostream>
#include <path.h>

irr::scene::ISceneManager* App::m_smgr = nullptr;
irr::video::IVideoDriver* App::m_driver = nullptr;
std::vector<irr::scene::IMeshSceneNode*> App::m_boxes;


bool App::isNumber(int index, lua_State * L, float *number)
{
	bool isNR = false;
	lua_pushnumber(L, index);
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1)) {
		*number = lua_tonumber(L, -1);
		isNR = true;
	}
	int i = lua_istable(L, -2);
	lua_pop(L, 1);
	return isNR;
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
	
	m_smgr->getActiveCamera()->setPosition(irr::core::vector3df(0, 0, 30));
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
	//m_smgr->getActiveCamera()->setTarget(irr::core::vector3df(0, 0, 0));

	if (m_device->isWindowActive()) {
		m_driver->beginScene(true, true, irr::video::SColor(255, 90, 101, 140));

		m_smgr->drawAll();
		m_guienv->drawAll();

		m_driver->endScene();
	}
	else {
		m_device->yield();
	}
}

int App::addMesh(lua_State * L)
{
	return 0;
}

int App::addBox(lua_State * L)
{
	irr::core::vector3df ori;
	int size;
	std::string name;
	bool correct = false;

	if (lua_isstring(L, -1) != 0) {
		name = lua_tostring(L, -1);
		lua_pop(L, 1);
		if (lua_isnumber(L, -1)) {
			size = lua_tonumber(L, -1);
			lua_pop(L, 1);
			if (lua_istable(L, -1)) {
				/*lua_pushnumber(L, 1);
				lua_gettable(L, -2);
				if (lua_isnumber(L, -1))
					ori.X = lua_tonumber(L, -1);
				lua_pop(L, 2);*/
				if (isNumber(1, L, &ori.X)) {
					std::cout << "X" << std::endl;
					if (isNumber(2, L, &ori.Y)) {
						std::cout << "Y" << std::endl;
						if (isNumber(3, L, &ori.Z)) {
							std::cout << "Z" << std::endl;
							correct = true;
						}
					}
				}
					
				
					
			}
		}
	}

	else {

	}
	
	if (correct) {
		m_boxes.push_back(m_smgr->addCubeSceneNode(size, 0, 4, ori, irr::core::vector3df(0, 0, 0), irr::core::vector3df(1, 1, 1)));
		//m_boxes[m_boxes.size() - 1]->setName(&name);
		m_boxes.operator[](m_boxes.size() - 1)->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		m_boxes.operator[](m_boxes.size() - 1)->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);
		std::cout << "succes" << std::endl;
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
		if (isNumber(1, L, &target.X)) {
			if (isNumber(2, L, &target.Y)) {
				if (isNumber(3, L, &target.Z)) {
					isPos = true;
				}
			}
		}
		debug = lua_gettop(L);
		lua_pop(L, 1);

		if (lua_istable(L, -1)) {
			if (isNumber(1, L, &pos.X)) {
				if (isNumber(2, L, &pos.Y)) {
					if (isNumber(3, L, &pos.Z)) {
						isTarget = true;
					}
				}
			}
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
	if (lua_isstring(L, -1)) {
		std::string name = lua_tostring(L, -1);
		irr::video::IImage *screenshot = m_driver->createScreenShot();
		//irr::io::path path = irr::core::string<irr::c8>(name);
		bool worked = m_driver->writeImageToFile(screenshot, path);
		std::cout << worked << std::endl;
	}
	
	return 0;
}

lua_State * App::getLuaState()
{
	return this->L;
}
