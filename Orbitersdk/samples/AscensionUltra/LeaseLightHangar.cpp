// Copyright (C) 2010-2014 Friedrich Kastner-Masilko
//
// This file is part of the Ascension Ultra Project
//
// Ascension Ultra is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of
// the License, or (at your option) any later version.
//
// Ascension Ultra is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Ascension Ultra. If not, see
// <http://www.gnu.org/licenses/>.

#include "LeaseLightHangar.h"

int LeaseLightHangar::GetType(){return HANGARTYPELL;}

void LeaseLightHangar::DefineAnimations ()
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

int LeaseLightHangar::GetDoors(){return LLDOORS;}

Door *LeaseLightHangar::GetDoor(int index){return (index>=0 && index<LLDOORS)?doors+index:NULL;}

void LeaseLightHangar::SetPosition(VECTOR3 position){this->position=position;}

int LeaseLightHangar::InitActionAreas(UMMUCREWMANAGMENT *crew, int index)
{
	VECTOR3 areas[2]={LLAREA_MAIN, LLAREA_ACCESS};
	areaStart=index;
	for(int i=0;i<LLDOORS;i++) doors[i].LinkActionArea(crew, index++, position+areas[i], 5);
	areaEnd=index-1;
	return index;
}

bool LeaseLightHangar::ActionAreaActivated(int action)
{
	if (action<areaStart || action>areaEnd) return false;
	int door=action-areaStart;
	if (doors[door].GetPosition()<=0) doors[door].Open();
	else doors[door].Close();
	return true;
}