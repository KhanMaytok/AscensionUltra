#include "Room.h"
#include "Hangar.h"

Room::Room()
{
	hangar = NULL;
	name = NULL;
	docked = NULL;
}

void Room::Init(VESSEL *owner, Hangar *hangar, const char *name, VECTOR3 cameraPosition, VECTOR3 viewDirection, VECTOR3 doorPosition, int capacity)
{
	crew.InitUmmu(owner->GetHandle());
	doorPosition+=hangar->GetPosition();
	VECTOR3 n=_V(-1,0,-1)+doorPosition;
	VECTOR3 p=_V(1,2,1)+doorPosition;
	crew.DefineAirLockShape(true, n.x,p.x,n.y,p.y,n.z,p.z);
	VECTOR3 d=_V(0,0,1)-viewDirection;
	crew.SetMembersPosRotOnEVA(_V(0,1,0)+doorPosition, _V(0,asin(length(d)/(d.x<0?2:-2))*2,0));
	this->capacity=capacity;
	crew.SetMaxSeatAvailableInShip(capacity);
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
		sprintf(oapiDebugString(),"%s \"%s\" aged %i entered %s through %s",
		crew.GetCrewMiscIdByName(crew.GetLastEnteredCrewName()),crew.GetLastEnteredCrewName()
		,crew.GetCrewAgeByName(crew.GetLastEnteredCrewName()),hangar->GetName(), name);
		break;
	case UMMU_TRANSFERED_TO_OUR_SHIP:
		VESSEL *vessel=oapiGetFocusInterface();
		if (vessel!=NULL && vessel==GetDock())
		{
			sprintf(oapiDebugString(),"%s \"%s\" aged %i transfered to %s at %s",
			crew.GetCrewMiscIdByName(crew.GetLastEnteredCrewName()),crew.GetLastEnteredCrewName()
			,crew.GetCrewAgeByName(crew.GetLastEnteredCrewName()), name, hangar->GetName());
		}
		else crew.RemoveCrewMember(crew.GetLastEnteredCrewName()); //Redundant transfer, we need to remove it
		break;
	}
}

Room::~Room(){delete [] name;}
Hangar *Room::GetHangar(){return hangar;}
char *Room::GetName(){return name;}
VECTOR3 Room::GetCameraPosition(){return cameraPosition;}
VECTOR3 Room::GetViewDirection(){return viewDirection;}
UMMUCREWMANAGMENT *Room::GetCrew(){return &crew;}
int Room::GetMaxPersons(){return capacity;}
void Room::SetDock(VESSEL *vessel){docked = vessel;}
VESSEL *Room::GetDock(){return docked;}