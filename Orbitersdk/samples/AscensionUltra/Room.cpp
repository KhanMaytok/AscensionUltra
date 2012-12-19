#include "Room.h"
#include "Hangar.h"
#include <map>

//Static member holds transfer process per vessel
//  This is a workaround for UMMU lack of multi-crew support
static std::map<VESSEL *, VesselRoomTransfer> vesselRoomTransfer;

Room::Room()
{
	owner = NULL;
	hangar = NULL;
	name = NULL;
	docked = NULL;
}

void Room::Init(VESSEL *owner, Hangar *hangar, const char *name, VECTOR3 cameraPosition, VECTOR3 viewDirection, VECTOR3 doorPosition, int capacity, VECTOR3 EVADirection)
{
	this->owner=owner;
	this->capacity=capacity;
	if (capacity>0);
	{
		crew.InitUmmu(owner->GetHandle());
		doorPosition+=hangar->GetPosition();
		VECTOR3 n=_V(-1,0,-1)+doorPosition;
		VECTOR3 p=_V(1,2,1)+doorPosition;
		crew.DefineAirLockShape(true, n.x,p.x,n.y,p.y,n.z,p.z);
		VECTOR3 d=_V(0,0,1)-viewDirection;
		if (EVADirection.x==DBL_MAX && EVADirection.y==DBL_MAX && EVADirection.z==DBL_MAX)
			EVADirection=_V(0,asin(length(d)/(d.x<0?2:-2))*2,0);
		crew.SetMembersPosRotOnEVA(_V(0,1.5,0)+doorPosition, EVADirection);		
		crew.SetMaxSeatAvailableInShip(capacity);
	}
	this->hangar=hangar;
	strcpy(this->name=new char[strlen(name)+1], name);
	this->cameraPosition=cameraPosition;
	this->viewDirection=viewDirection;
	ResetTransfered();
}

void Room::PostStep (double simt, double simdt, double mjd)
{
	if (capacity>0)
	{
		int process=crew.ProcessUniversalMMu();
		switch (process)
		{
		case UMMU_RETURNED_TO_OUR_SHIP:
			break;	
		default:
			VesselRoomTransfer *transfer=GetTransfered();
			if (oapiGetFocusInterface()==docked) transfer->Room=this;
			if (process==UMMU_TRANSFERED_TO_OUR_SHIP)
			{
				//Put transfered crew into transfer struct
				char *s=crew.GetLastEnteredCrewName();
				int l=strlen(s);
				if (l>0)
				{
					//Check and fill out name
					if (transfer->Name==NULL)
					{
						transfer->Name=new char[l+1];
						strcpy(transfer->Name, s);
						transfer->Name[l]=0;
					}
					//Check and fill out misc ID
					char *id=crew.GetCrewMiscIdByName(s);
					int idl=strlen(id);
					if (transfer->MiscId==NULL && idl>0)
					{
						transfer->MiscId=new char[idl+1];
						strcpy(transfer->MiscId, id);
						transfer->MiscId[idl]=0;
					}
					//Check and fill out numbers
					int v=crew.GetCrewAgeByName(s);
					if (v>=0) transfer->Age=v;
					v=crew.GetCrewPulseByName(s);
					if (v>=0) transfer->Puls=v;
					v=crew.GetCrewWeightByName(s);
					if (v>=0) transfer->Weight=v;				
				}
				crew.RemoveCrewMember(s); //Redundant transfer, we need to remove it			
			}		
			break;
		}
	}
	PostPostStep();
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
VesselRoomTransfer *Room::GetTransfered(){return &vesselRoomTransfer[owner];}
void Room::ResetTransfered()
{
	VesselRoomTransfer *transfer=&vesselRoomTransfer[owner];
	delete [] transfer->Name;
	delete [] transfer->MiscId;
	memset(transfer, 0, sizeof(VesselRoomTransfer));
}
void Room::PostPostStep(){}