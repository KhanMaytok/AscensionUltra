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

#include "Taxiways.h"

Taxiways::Taxiways(void)
{	
}

Taxiways::~Taxiways(void)
{
	for(std::vector<BeaconArray *>::iterator i=beacons.begin();i!=beacons.end();i++) delete *i;
	for(std::vector<BeaconPath *>::iterator i=paths.begin();i!=paths.end();i++) delete *i;
	for(std::vector<char *>::iterator i=endPoints.begin();i!=endPoints.end();i++) delete [] *i;
}

void Taxiways::Init(VESSEL* owner, const char *ini, const char *classname, VECTOR3 &position)
{
	ReadBeaconDefinition(beacons, ini, classname, position, owner);
	ReadBeaconPaths(paths, beacons, ini, classname, owner);
	ReadBeaconEndPoints(endPoints, ini, classname);
	Routes::Init(
		beacons[0]->GetSize(),
		beacons[0]->GetFallOff(),
		beacons[0]->GetPeriod(),
		beacons[0]->GetDuration(),
		beacons[0]->GetPropagate());
	ReadBeaconRoutes(*this, paths, endPoints, ini, classname);
	Switch(true);
	PriorityFinalize();
}
