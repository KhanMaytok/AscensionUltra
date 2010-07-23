#include "Room.h"

Room::Room()
{
	owner = NULL;
	name = NULL;
}

void Room::Init(Hangar *owner, const char *name, VECTOR3 cameraPosition, VECTOR3 viewDirection)
{
	this->owner=owner;
	strcpy(this->name=new char[strlen(name)+1], name);
	this->cameraPosition=cameraPosition;
	this->viewDirection=viewDirection;
}

Room::~Room()
{
	delete [] name;
}

Hangar *Room::GetHangar(){return owner;}
char *Room::GetName(){return name;}
VECTOR3 Room::GetCameraPosition(){return cameraPosition;}
VECTOR3 Room::GetViewDirection(){return viewDirection;}