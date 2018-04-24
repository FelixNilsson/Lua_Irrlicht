#include "nodemanager.h"

NodeManager::NodeManager(irr::scene::ISceneManager * p)
{
	this->smgr = p;
}

int NodeManager::addMesh(lua_State * L)
{
	return 0;
}

int NodeManager::addBox(lua_State * L)
{
	return 0;
}

int NodeManager::getNodes(lua_State * L)
{
	return 0;
}
