#pragma once

#include <lua.hpp>
#include <irrlicht.h>
#include <vector>

class App {
	struct vectorUV {
		irr::core::vector3df vec;
		irr::core::vector2df uv;
	};
private:
	lua_State * L;

	static irr::IrrlichtDevice* m_device;
	static irr::video::IVideoDriver* m_driver;
	static irr::scene::ISceneManager* m_smgr;
	static irr::gui::IGUIEnvironment* m_guienv;

	static std::vector<irr::scene::IMeshSceneNode*> m_meshes;
	static std::vector<irr::scene::IMeshSceneNode*> m_boxes;
	static int m_id;
	void setupScript();
public:
	
	
private:
	static bool isVector(lua_State * L, irr::core::vector3df &vector);
	static bool isVectorUV(lua_State * L, vectorUV &vector);
	static void drawOneFrame();
	static bool isPowerOfTwo(int nr);
	static irr::scene::ISceneNode* getSceneNode(std::string nodeName);
	static void destroyScene();
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
	static int addTexture(lua_State* L);
	static int bind(lua_State* L);
	static int loadScene(lua_State* L);
	lua_State* getLuaState();
};