// Copyright (C) 2010-2014 Friedrich Kastner-Masilko
//
// Contributors:
//              Janez aka RisingFury - animation code
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
#include "orbitersdk.h"
#include <vector>
#include "BeaconArray.h"
#include "Hangar.h"

class Tracker: public Hangar
{
public:
	void clbkPostStep (double simt, double simdt, double mjd);
	void clbkPostCreation ();
	void DefineAnimations();
	bool clbkLoadStateEx (char *line);
	void clbkSaveState (FILEHANDLE scn);
	bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);
	void SetPosition(VECTOR3 position);

	virtual void SetTarget(OBJHANDLE target);
	virtual OBJHANDLE GetTarget();
	virtual double GetDistance();
	virtual double GetSpeed();
	
private:
	MGROUP_ROTATE *mgroupAzimuth;
	MGROUP_ROTATE *mgroupElevation;
	int anim_azimuth, anim_elevation;
	OBJHANDLE target;
	double rotationOffset;
	double VerticalAxisRotationRate;			// Rotation rate around the vertical axis. This changes the azimuth.
	double HorizontalAxisRotationRate;			// Rotation rate around the horizontal axis. This changes the elevation. ATTENTION: Currently the two rates are hard coded. Feel free to change them or load from config.
	double CurrentElevation;			// ATTENTION: Current initial values for azimuth and elevation are 0 and 0. You'll have to change this to load them from scenario / config and save them on exit, so that the dish starts where it was when simulation ended. 
	double CurrentAzimuth;
};