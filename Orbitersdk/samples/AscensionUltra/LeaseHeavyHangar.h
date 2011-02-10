// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// LeaseHeavyHangar.h
// Class definition of heavy lease hangar building.
// ==============================================================
#pragma once
#include "Hangar.h"
#include "Door.h"
#define DOORS 1
#define AREA_MAIN _V(43,0,-49.5)

class LeaseHeavyHangar:public Hangar
{
public:
	int GetType();
	void DefineAnimations ();
	int GetDoors();
	Door *GetDoor(int index);
	void SetPosition(VECTOR3 position);
	int InitActionAreas(UMMUCREWMANAGMENT *crew, int index);
	bool ActionAreaActivated(int action);
private:
	Door doors[DOORS];
};
