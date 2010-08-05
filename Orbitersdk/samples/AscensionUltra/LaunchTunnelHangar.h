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
#define DOORS 4

class LaunchTunnelHangar:public Hangar
{
public:
	HangarType GetType();
	void DefineAnimations ();
	int GetDoors();
	Door *GetDoor(int index);	
private:
	Door doors[DOORS];	
};
