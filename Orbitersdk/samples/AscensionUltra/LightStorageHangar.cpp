// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// LightStorageHangar.cpp
// Class implementation of light storage hangar building.
// ==============================================================
#include "LightStorageHangar.h"

void LightStorageHangar::DefineAnimations ()
{
	static UINT DoorGrp[DOORS] = {0,1,2,3};
	char prefix[20]="";
	int i=0;

	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[0].Init(owner, new MGROUP_ROTATE(meshIndex, DoorGrp, 1,	_V(0,0,0), _V(-1,0,0), (float)(45*RAD)), prefix);
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[1].Init(owner, new MGROUP_ROTATE(meshIndex, DoorGrp+1, 1,	_V(0,0,0), _V(1,0,0), (float)(45*RAD)), prefix);
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[2].Init(owner, new MGROUP_TRANSLATE(meshIndex, DoorGrp+2, 1, _V(0,3.62,0)), prefix);
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[3].Init(owner, new MGROUP_TRANSLATE(meshIndex, DoorGrp+3, 1, _V(0,1.81,0)), prefix);
	Hangar::DefineAnimations();
}

int LightStorageHangar::GetDoors(){return DOORS;}

Door *LightStorageHangar::GetDoor(int index){return (index>=0 && index<DOORS)?doors+index:NULL;}