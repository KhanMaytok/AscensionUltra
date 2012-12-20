// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// LaunchTunnel.cpp
// Class implementation of launch tunnel building.
// ==============================================================
#include "LaunchTunnelHangar.h"
#include "Module.h"

int LaunchTunnelHangar::GetType(){return HANGARTYPELFMC;}

void LaunchTunnelHangar::DefineAnimations ()
{
	static UINT DoorGrp[4] = {0,1,2,3};
	char prefix[20]="";
	int i=0;
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);	
	doors[0].Init(owner, "Main Door", prefix, 1,
		new MGROUP_ROTATE(meshIndex, DoorGrp, 1,	_V(223,0,0), _V(0,0,1), (float)(45*RAD)));
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);	
	doors[1].Init(owner, "Exit Door", prefix, 1,
		new MGROUP_ROTATE(meshIndex, DoorGrp+1, 1,	_V(0,0,0), _V(0,0,1), (float)(-45*RAD)));
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);	
	doors[2].Init(owner, "Blast Shield", prefix, 1,
		new MGROUP_ROTATE(meshIndex, DoorGrp+2, 1,	_V(-189.3,0,47.5), _V(0,1,0), (float)(-90*RAD)));
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[3].Init(owner, "Tunnel Door", prefix, 1,
		new MGROUP_TRANSLATE(meshIndex, DoorGrp+3, 1,	_V(-47,0,0)));

	rooms[0].Init(owner, this, "Control Room", _V(111,20,47), _V(0,0,-1), _V(115,0,50), 20);
	rooms[1].Init(owner, this, "Control Tower", _V(111,218,70), _V(0,0,-1));

	Hangar::DefineAnimations();
}

int LaunchTunnelHangar::GetDoors(){return DOORS;}

Door *LaunchTunnelHangar::GetDoor(int index){return (index>=0 && index<DOORS)?doors+index:NULL;}

int LaunchTunnelHangar::GetRooms(){return ROOMS;}

Room *LaunchTunnelHangar::GetRoom(int index){return (index>=0 && index<ROOMS)?rooms+index:NULL;}

bool LaunchTunnelHangar::CheckVincinity(VECTOR3 *pos){return pos->x>position.x+85 && pos->x<position.x+145 && pos->z<position.z+30 && pos->z>position.z-30;}

void LaunchTunnelHangar::SetPosition(VECTOR3 position){this->position=position;}