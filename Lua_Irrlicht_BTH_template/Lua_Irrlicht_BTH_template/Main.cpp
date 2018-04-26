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

	
	std::cout << "I'm helping\n";
	

	//mesh stuff
	/*irr::scene::IAnimatedMesh* mesh = smgr->getMesh("../../Bin/Meshes/sydney.md2");
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
	}*/
	
	//smgr->addCameraSceneNode(0, irr::core::vector3df(0, 30, -40), irr::core::vector3df(0, 5, 0));
	
	//end mesh stuff
	
	
	
	while(app.run()) {
		app.draw();	
	}

	//device->drop();

	conThread.join();
	return 0;
}