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
#include "Door.h"
#include "Group.h"

class VerticalLaunchDoor:public Door
{
public:
	VerticalLaunchDoor();
	~VerticalLaunchDoor();
	void InitBeaconTransform(Group *group, int transforms, ... );
	void DefineAnimations();
	void PostStep (double simt, double simdt, double mjd);
private:
	Group *beaconGroups;
	int beaconTransforms;
	MGROUP_TRANSFORM **beaconTransform;
};
