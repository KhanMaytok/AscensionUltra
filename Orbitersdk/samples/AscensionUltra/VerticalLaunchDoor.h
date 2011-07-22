// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// VerticalLaunchDoor.h
// Class definition of automatic "door" sequence for vertical launch facility MLP and service structure controller.
// ==============================================================

#pragma once
#include "Door.h"

class VerticalLaunchDoor:public Door
{
public:
	void DefineAnimations();
};
