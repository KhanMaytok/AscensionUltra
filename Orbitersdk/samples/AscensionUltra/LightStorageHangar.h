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
#define DOORS 4

class LightStorageHangar:public Hangar
{
public:
	HangarType GetType();
	void DefineAnimations ();
	int GetDoors();
	Door *GetDoor(int index);	
private:
	Door doors[DOORS];
};
