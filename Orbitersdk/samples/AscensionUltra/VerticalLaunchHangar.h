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

class VerticalLaunchHangar:public Hangar
{
public:
	int GetType();
	void DefineAnimations ();
};
