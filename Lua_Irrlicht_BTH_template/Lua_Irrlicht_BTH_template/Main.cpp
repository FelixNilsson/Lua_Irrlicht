#pragma comment(lib, "Irrlicht.lib")
#ifdef _DEBUG
#pragma comment(lib, "LuaLibd.lib")
#else
#pragma comment(lib, "Lualib.lib")
#endif

#include <lua.hpp>
#include <Windows.h>
#include <iostream>
#include <thread>
#include "lua.hpp"
#include <irrlicht.h>
#include "app.h"

void ConsoleThread(lua_State* L) {
	char command[1000];
	while(GetConsoleWindow()) {
		memset(command, 0, 1000);
		std::cin.getline(command, 1000);
		if( luaL_loadstring(L, command) || lua_pcall(L, 0, 0, 0) )
			std::cout << lua_tostring(L, -1) << '\n';
	}
}


int main()
{
	App app;
	std::thread conThread(ConsoleThread, app.getLuaState());

	irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_SOFTWARE, irr::core::dimension2d<irr::u32>(640, 480), 16, false, false, true, 0);
	if(!device)
		return 1;

	device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");
	irr::video::IVideoDriver* driver	= device->getVideoDriver();
	irr::scene::ISceneManager* smgr		= device->getSceneManager();
	irr::gui::IGUIEnvironment* guienv	= device->getGUIEnvironment();
	

	guienv->addStaticText(L"Hello World! This is the Irrlicht Software renderer!", irr::core::rect<irr::s32>(10, 10, 260, 22), true);

	//mesh stuff
	irr::scene::IAnimatedMesh* mesh = smgr->getMesh("../../Bin/Meshes/sydney.md2");
	if (!mesh)
	{
		device->drop();
		return 1;
	}
	irr::scene::IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(mesh);
	if (node)
	{
		node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		node->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, false);
		node->setMD2Animation(irr::scene::EMAT_STAND);
		node->setMaterialTexture(0, driver->getTexture("../../Bin/Meshes/sydney.bmp"));
	}
	
	//smgr->addCameraSceneNode(0, irr::core::vector3df(0, 30, -40), irr::core::vector3df(0, 5, 0));
	
	//end mesh stuff
	
	irr::SKeyMap keymap[4];
	keymap[0].Action = irr::EKA_MOVE_FORWARD;
	keymap[0].KeyCode = irr::KEY_KEY_W;
	keymap[1].Action = irr::EKA_MOVE_BACKWARD;
	keymap[1].KeyCode = irr::KEY_KEY_S;
	keymap[2].Action = irr::EKA_STRAFE_LEFT;
	keymap[2].KeyCode = irr::KEY_KEY_A;
	keymap[3].Action = irr::EKA_STRAFE_RIGHT;
	keymap[3].KeyCode = irr::KEY_KEY_D;

	smgr->addCameraSceneNodeFPS(0, 100, 0.5, -1, keymap, 4);
	
	while(device->run()) {
		if (device->isWindowActive()) {
			driver->beginScene(true, true, irr::video::SColor(255, 90, 101, 140));

			smgr->drawAll();
			guienv->drawAll();

			driver->endScene();
		}
		else {
			device->yield();
		}
		//node->setPosition(pos);
			
	}

	device->drop();

	conThread.join();
	return 0;
}