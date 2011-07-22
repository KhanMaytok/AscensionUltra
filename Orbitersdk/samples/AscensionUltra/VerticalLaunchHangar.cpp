// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// VerticalLaunchHangar.cpp
// Class implementation of vertical launch facility buildings.
// ==============================================================
#include "VerticalLaunchHangar.h"

int VerticalLaunchHangar::GetType(){return HANGARTYPEVLC;}

void VerticalLaunchHangar::DefineAnimations ()
{
	static UINT DoorGrp[9] = {2,0,1,3,4,5,6,7,8};
	char prefix[40]="";
	int i=0;

	sprintf(prefix, "%sDOOR0", event_prefix);
	doors[0].Init(owner, "Service Structure & MLP", prefix, 8,
		new MGROUP_TRANSLATE(meshIndex, DoorGrp, 1, _V(0,0,290)),
		new MGROUP_TRANSLATE(meshIndex, DoorGrp+1, 2, _V(0,0,-290)),
		new MGROUP_ROTATE(meshIndex, DoorGrp+3, 1,	_V(-17,32,-97), _V(0,0,1), (float)(90*RAD)),
		new MGROUP_ROTATE(meshIndex, DoorGrp+4, 1,	_V(-17,52,-97), _V(0,0,1), (float)(90*RAD)),
		new MGROUP_ROTATE(meshIndex, DoorGrp+5, 1,	_V(-17,92,-97), _V(0,0,1), (float)(90*RAD)),
		new MGROUP_ROTATE(meshIndex, DoorGrp+6, 1,	_V(17,32,-97), _V(0,0,-1), (float)(90*RAD)),
		new MGROUP_ROTATE(meshIndex, DoorGrp+7, 1,	_V(17,52,-97), _V(0,0,-1), (float)(90*RAD)),
		new MGROUP_ROTATE(meshIndex, DoorGrp+8, 1,	_V(17,92,-97), _V(0,0,-1), (float)(90*RAD)));
	Hangar::DefineAnimations();
	
	rooms[0].Init(owner, this, "Vertical Launch Control", _V(0,129,-137), _V(0,50,195), _V(0,0,0), 4);
}

int VerticalLaunchHangar::GetDoors(){return DOORS;}

Door *VerticalLaunchHangar::GetDoor(int index){return (index>=0 && index<DOORS)?doors+index:NULL;}

int VerticalLaunchHangar::GetRooms(){return ROOMS;}

Room *VerticalLaunchHangar::GetRoom(int index){return (index>=0 && index<ROOMS)?rooms+index:NULL;}