#pragma once

#include <lua.hpp>
#include <irrlicht.h>

class NodeManager {
private:
	irr::scene::ISceneManager* smgr;
public:
	NodeManager(irr::scene::ISceneManager* p);
	static int addMesh(lua_State* L);
	static int addBox(lua_State* L);
	static int getNodes(lua_State* L);
};