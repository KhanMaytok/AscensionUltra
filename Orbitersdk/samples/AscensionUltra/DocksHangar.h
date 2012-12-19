// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// DocksHangar.h
// Class definition of dockyard buildings.
// ==============================================================
#pragma once
#include "Hangar.h"

class DocksHangar:public Hangar
{
public:
	int GetType();
	void SetPosition(VECTOR3 position);
};
