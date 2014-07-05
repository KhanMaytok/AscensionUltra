// Copyright (C) 2010-2014 Friedrich Kastner-Masilko
//
// This file is part of the Ascension Ultra Project
//
// Ascension Ultra is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of
// the License, or (at your option) any later version.
//
// Ascension Ultra is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Ascension Ultra. If not, see
// <http://www.gnu.org/licenses/>.

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
