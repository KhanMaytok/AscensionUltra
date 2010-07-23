// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// LightStorageHangar.h
// Class definition of light storage hangar building.
// ==============================================================
#pragma once
#include "Hangar.h"
#include "Door.h"
#include "Room.h"
#define DOORS 2
#define ROOMS 1

class LightStorageHangar:public Hangar
{
public:
	HangarType GetType();
	void DefineAnimations ();
	int GetDoors();
	Door *GetDoor(int index);
	int GetRooms();
	Room *GetRoom(int index);
	VECTOR3 GetPosition();
	void SetPosition(VECTOR3 position);
private:
	Door doors[DOORS];
	Room rooms[ROOMS];
	VECTOR3 position;
};
