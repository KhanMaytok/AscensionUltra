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

HangarType LightStorageHangar::GetType(){return HangarType::LightStorage;}

void LightStorageHangar::DefineAnimations ()
{
	static UINT DoorGrp[4] = {0,1,2,3};
	char prefix[20]="";
	int i=0;
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);	
	doors[0].Init(owner, "Main Door", prefix, 2,
		new MGROUP_ROTATE(meshIndex, DoorGrp+2, 1,	_V(0,0,0), _V(0,1,0), (float)(45*RAD)),
		new MGROUP_ROTATE(meshIndex, DoorGrp+3, 1,	_V(0,0,0), _V(0,-1,0), (float)(45*RAD)));
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);
	doors[1].Init(owner, "Maintenance Door", prefix, 2,
		new MGROUP_TRANSLATE(meshIndex, DoorGrp, 1, _V(0,3.62,0)),
		new MGROUP_TRANSLATE(meshIndex, DoorGrp+1, 1, _V(0,1.81,0)));
	Hangar::DefineAnimations();
}

int LightStorageHangar::GetDoors(){return DOORS;}

Door *LightStorageHangar::GetDoor(int index){return (index>=0 && index<DOORS)?doors+index:NULL;}