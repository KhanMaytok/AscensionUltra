#include "Room.h"
#include "Hangar.h"

Room::Room()
{
	hangar = NULL;
	name = NULL;
}

void Room::Init(VESSEL *owner, Hangar *hangar, const char *name, VECTOR3 cameraPosition, VECTOR3 viewDirection, VECTOR3 doorPosition)
{
	crew.InitUmmu(owner->GetHandle());
	doorPosition+=hangar->GetPosition();
	VECTOR3 n=_V(-10,-10,-10)+doorPosition;
	VECTOR3 p=_V(10,10,10)+doorPosition;
	crew.DefineAirLockShape(true, n.x,p.x,n.y,p.y,n.z,p.z);
	crew.SetMaxSeatAvailableInShip(4);
	this->hangar=hangar;
	strcpy(this->name=new char[strlen(name)+1], name);
	this->cameraPosition=cameraPosition;
	this->viewDirection=viewDirection;
}

void Room::PostStep (double simt, double simdt, double mjd)
{
	switch (crew.ProcessUniversalMMu())
	{
	case UMMU_RETURNED_TO_OUR_SHIP:
	case UMMU_TRANSFERED_TO_OUR_SHIP:
		sprintf(oapiDebugString(),"%s -> %s->%s(%d)", crew.GetLastEnteredCrewName(), hangar->GetName(), name, crew.GetCrewTotalNumber());
		break;
	}
}

Room::~Room(){delete [] name;}
Hangar *Room::GetHangar(){return hangar;}
char *Room::GetName(){return name;}
VECTOR3 Room::GetCameraPosition(){return cameraPosition;}
VECTOR3 Room::GetViewDirection(){return viewDirection;}