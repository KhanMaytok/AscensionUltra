// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// SmallVerticalLaunchHangar.h
// Class definition of small vertical launch facility buildings.
// ==============================================================
#pragma once
#include "Hangar.h"
#include "Room.h"
#include "VerticalLaunchDoor.h"
#include "BeaconArray.h"
#include <vector>

#define DOORS	1
#define ROOMS	1

class SmallVerticalLaunchHangar:public Hangar
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
	VerticalLaunchDoor doors[DOORS];
	Room rooms[ROOMS];
};