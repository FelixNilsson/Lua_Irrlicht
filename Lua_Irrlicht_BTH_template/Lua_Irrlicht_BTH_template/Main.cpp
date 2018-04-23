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

void ConsoleThread(lua_State* L) {
	char command[1000];
	while(GetConsoleWindow()) {
		memset(command, 0, 1000);
		std::cin.getline(command, 1000);
		if( luaL_loadstring(L, command) || lua_pcall(L, 0, 0, 0) )
			std::cout << lua_tostring(L, -1) << '\n';
	}
}

irr::core::vector3df pos(0, 0, 0);

int updatepos(lua_State* L) {
	pos.X += lua_tonumber(L, 1);
	pos.Y += lua_tonumber(L, 2);
	pos.Z += lua_tonumber(L, 3);
	return 0;
}

int getpos(lua_State* L) {
	lua_newtable(L);
	lua_pushstring(L, "1");
	lua_pushnumber(L, pos.X);
	lua_settable(L, -3);
	lua_pushstring(L, "2");
	lua_pushnumber(L, pos.Y);
	lua_settable(L, -3);
	lua_pushstring(L, "3");
	lua_pushnumber(L, pos.Z);
	lua_settable(L, -3);
	return 1;
}

int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	lua_pushcfunction(L, updatepos);
	lua_setglobal(L, "updatepos");
	lua_pushcfunction(L, getpos);
	lua_setglobal(L, "getpos");

		std::thread conThread(ConsoleThread, L);

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

	smgr->addCameraSceneNodeFPS(0, 100, 500, -1, keymap, 4);

	while(device->run() && device->isWindowFocused()) {
		
		//node->setPosition(pos);
		driver->beginScene(true, true, irr::video::SColor(255, 90, 101, 140));

		smgr->drawAll();
		guienv->drawAll();

		driver->endScene();		
	}

	device->drop();

	conThread.join();
	return 0;
}