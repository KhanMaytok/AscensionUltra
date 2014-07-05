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
#include "Routes.h"

class Runways :
	public Routes
{
public:
	Runways(void);
	~Runways(void);
	void Init(VESSEL* owner, const char *ini, const char *classname, VECTOR3 &position);
	virtual void Activate(int index);
	virtual bool On(const char *point, bool isEnd=false);
	virtual int GetPoints(bool isEnd=false, char *fromPoint=NULL);
private:
	std::vector<char *> endPoints;
	std::vector<BeaconArray *> beacons;
	std::vector<BeaconPath *> paths;
	int staticIndex;
};
