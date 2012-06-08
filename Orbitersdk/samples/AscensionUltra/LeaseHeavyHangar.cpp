// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// LeaseHeavyHangar.cpp
// Class implementation of heavy lease hangar building.
// ==============================================================
#include "LeaseHeavyHangar.h"

int LeaseHeavyHangar::GetType(){return HANGARTYPELH;}

void LeaseHeavyHangar::DefineAnimations ()
{
	static UINT DoorGrp[1] = {0};
	char prefix[20]="";
	int i=0;
	sprintf(prefix, "%sDOOR%d", event_prefix, i++);	
	doors[0].Init(owner, "Main Door", prefix, 1,
		new MGROUP_ROTATE(meshIndex, DoorGrp, 1, _V(0,0,0), _V(1,0,0), (float)(35*RAD)));	
	Hangar::DefineAnimations();
}

int LeaseHeavyHangar::GetDoors(){return DOORS;}

Door *LeaseHeavyHangar::GetDoor(int index){return (index>=0 && index<DOORS)?doors+index:NULL;}

void LeaseHeavyHangar::SetPosition(VECTOR3 position){this->position=position;}

int LeaseHeavyHangar::InitActionAreas(UMMUCREWMANAGMENT *crew, int index)
{
	VECTOR3 areas[1]={AREA_MAIN};
	areaStart=index;
	for(int i=0;i<DOORS;i++) doors[i].LinkActionArea(crew, index++, position+areas[i], 5);
	areaEnd=index-1;
	return index;
}

bool LeaseHeavyHangar::ActionAreaActivated(int action)
{
	if (action<areaStart || action>areaEnd) return false;
	int door=action-areaStart;
	if (doors[door].GetPosition()<=0) doors[door].Open();
	else doors[door].Close();
	return true;
}