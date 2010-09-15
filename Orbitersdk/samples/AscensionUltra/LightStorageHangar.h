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
#define DOORS 2
#define AREA_MAIN _V(14,0,-13.5)
#define AREA_ACCESS _V(-5.5,0,18.5)

class LightStorageHangar:public Hangar
{
public:
	HangarType GetType();
	void DefineAnimations ();
	int GetDoors();
	Door *GetDoor(int index);
	void SetPosition(VECTOR3 position);
	int InitActionAreas(UMMUCREWMANAGMENT *crew, int index);
	bool ActionAreaActivated(int action);
private:
	Door doors[DOORS];	
};
