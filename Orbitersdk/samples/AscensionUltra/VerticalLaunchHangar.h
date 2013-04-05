// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// VerticalLaunchHangar.h
// Class definition of vertical launch facility buildings.
// ==============================================================
#pragma once
#include "Hangar.h"
#include "Room.h"
#include "VerticalLaunchDoor.h"
#include "BeaconArray.h"
#include <vector>

#define VLCDOORS	1
#define VLCROOMS	1

class VerticalLaunchHangar:public Hangar
{
public:
	int GetType();
	void DefineAnimations ();
	int GetDoors();
	Door *GetDoor(int index);
	int GetRooms();
	Room *GetRoom(int index);
	void SetPosition(VECTOR3 position);
private:
	VerticalLaunchDoor doors[VLCDOORS];
	Room rooms[VLCROOMS];
};