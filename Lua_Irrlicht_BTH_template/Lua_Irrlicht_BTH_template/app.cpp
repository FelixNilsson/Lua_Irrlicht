#include "app.h"

irr::scene::ISceneManager* App::smgr = nullptr;

App::App()
{
	this->device = irr::createDevice(irr::video::EDT_SOFTWARE, irr::core::dimension2d<irr::u32>(640, 480), 16, false, false, true, 0);

	if (!device) {
		//error
	}
	device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");
	this->driver = this->device->getVideoDriver();
	this->guienv = this->device->getGUIEnvironment();
	smgr = this->device->getSceneManager();

	guienv->addStaticText(L"Hello World! This is the Irrlicht Software renderer!", irr::core::rect<irr::s32>(10, 10, 260, 22), true);

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
	
}

App::~App()
{
	this->device->drop();
}

bool App::run()
{
	return this->device->run();
}

void App::draw()
{
	if (this->device->isWindowActive()) {
		this->driver->beginScene(true, true, irr::video::SColor(255, 90, 101, 140));

		this->smgr->drawAll();
		this->guienv->drawAll();

		this->driver->endScene();
	}
	else {
		this->device->yield();
	}
}

int App::addMesh(lua_State * L)
{
	return 0;
}

int App::addBox(lua_State * L)
{
	int i = lua_istable(L, 1);
	int k = lua_gettop(L);
	lua_gettable(L, 1);
	int t = lua_gettop(L);
	smgr->addCubeSceneNode();
	return 0;
}

int App::getNodes(lua_State * L)
{
	return 0;
}

int App::camera(lua_State * L)
{
	return 0;
}

int App::snapshot(lua_State * L)
{
	return 0;
}

lua_State * App::getLuaState()
{
	return this->L;
}
