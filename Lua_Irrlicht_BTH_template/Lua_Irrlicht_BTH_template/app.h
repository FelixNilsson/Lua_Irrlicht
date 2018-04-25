#pragma once

#include <lua.hpp>
#include <irrlicht.h>

class App {
private:
	lua_State * L;

	irr::IrrlichtDevice* device;
	irr::video::IVideoDriver* driver;
	irr::scene::ISceneManager* smgr;
	irr::gui::IGUIEnvironment* guienv;
public:
	App();
	~App();
	bool run();
	void draw();
	static int addMesh(lua_State* L);
	static int addBox(lua_State* L);
	static int getNodes(lua_State* L);
	static int camera(lua_State* L);
	static int snapshot(lua_State* L);
	lua_State* getLuaState();
};