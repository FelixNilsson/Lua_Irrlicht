#pragma once

#include <lua.hpp>
#include <irrlicht.h>
#include <vector>

class App {
private:
	lua_State * L;

	irr::IrrlichtDevice* m_device;
	static irr::video::IVideoDriver* m_driver;
	irr::gui::IGUIEnvironment* m_guienv;
	
public:
	static irr::scene::ISceneManager* m_smgr;
	static std::vector<irr::scene::IMeshSceneNode*> m_boxes;
private:
	static bool isVector(lua_State * L, irr::core::vector3df *vector);
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