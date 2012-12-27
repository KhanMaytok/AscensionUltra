// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// LaunchTunnel.h
// Class definition of launch tunnel building.
// ==============================================================
#pragma once
#include "Hangar.h"
#include "Door.h"
#include "Room.h"
#include "BeaconArray.h"
#include <vector>
#include "Checklist.h"

#define DOORS	4
#define ROOMS	2

class PrepareChecklist:public Checklist
{
};

class LaunchChecklist:public Checklist
{
};

class LaunchTunnelHangar:public Hangar
{
public:
	int GetType();
	void DefineAnimations ();
	int GetDoors();
	Door *GetDoor(int index);	
	int GetRooms();
	Room *GetRoom(int index);
	bool CheckVincinity(VECTOR3 *pos);
	void SetPosition(VECTOR3 position);
	int GetChecklists();
	Checklist *GetChecklist(int index);
private:
	Door doors[DOORS];	
	Room rooms[ROOMS];
	PrepareChecklist prepare;
	LaunchChecklist launch;
};
