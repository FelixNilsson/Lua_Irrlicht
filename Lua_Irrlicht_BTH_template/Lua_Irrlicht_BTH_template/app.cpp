#include "app.h"

App::App()
{
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

int App::addMesh(lua_State * L)
{
	return 0;
}

int App::addBox(lua_State * L)
{
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
