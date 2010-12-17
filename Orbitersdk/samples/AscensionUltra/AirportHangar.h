// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// AirportHangar.h
// Class definition of airport building.
// ==============================================================
#pragma once
#include "Hangar.h"
#include "Room.h"
#include "TransferDefaultRoom.h"

class AirportHangar:public Hangar
{
public:
	int GetType();
	void DefineAnimations ();
	int GetRooms();
	Room *GetRoom(int index);
	Room *GetEntrance();
private:
	Room entrance;	
	TransferDefaultRoom hall;
};
