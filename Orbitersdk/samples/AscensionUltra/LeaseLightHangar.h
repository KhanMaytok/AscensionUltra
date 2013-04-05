// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// LeaseLightHangar.h
// Class definition of light lease hangar building.
// ==============================================================
#pragma once
#include "Hangar.h"
#include "Door.h"
#define LLDOORS 2
#define LLAREA_MAIN _V(-14,0,13.5)
#define LLAREA_ACCESS _V(5.5,0,-18.5)

class LeaseLightHangar:public Hangar
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
	Door doors[LLDOORS];	
};
