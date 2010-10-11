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
	VECTOR3 n=_V(-1,0,-1)+doorPosition;
	VECTOR3 p=_V(1,2,1)+doorPosition;
	crew.DefineAirLockShape(true, n.x,p.x,n.y,p.y,n.z,p.z);
	crew.SetMembersPosRotOnEVA(_V(0,1,0)+doorPosition, _V(0,0,0));
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
		sprintf(oapiDebugString(),"%s \"%s\" aged %i entered %s through %s",
		crew.GetCrewMiscIdByName(crew.GetLastEnteredCrewName()),crew.GetLastEnteredCrewName()
		,crew.GetCrewAgeByName(crew.GetLastEnteredCrewName()),hangar->GetName(), name);
		break;
	}
}

Room::~Room(){delete [] name;}
Hangar *Room::GetHangar(){return hangar;}
char *Room::GetName(){return name;}
VECTOR3 Room::GetCameraPosition(){return cameraPosition;}
VECTOR3 Room::GetViewDirection(){return viewDirection;}
UMMUCREWMANAGMENT *Room::GetCrew(){return &crew;}