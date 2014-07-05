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
#include "orbitersdk.h"
#include "Group.h"
#include "UMmuSDK.h"

#define MAXACTIONLEN 80

class Door
{
public:
	Door(void);
	~Door(void);
	void Init(VESSEL *owner, const char *name, const char *event_prefix, int transforms, ... );
	void SetSpeed(double speed);
	virtual void Stop();
	virtual void Open();
	virtual void Close();
	virtual double GetPosition();
	virtual double GetMovement();
	virtual void PostStep (double simt, double simdt, double mjd);
	virtual void DefineAnimations();
	bool clbkLoadStateEx (char *line);
	void clbkSaveState (FILEHANDLE scn);
	void clbkPostCreation ();
	bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);
	void LinkActionArea(UMMUCREWMANAGMENT *crew, int action, VECTOR3 position, double radius);
	virtual char *GetName();
	void SetStatusBeaconGroups(Group *warn, Group *open);

private:
	void RecordEvent(double command);
	int action;
	double speed;
	double command;
	char *event_prefix, *name, *actionText;
	UMMUCREWMANAGMENT *crew;
	Group *warn, *open;
protected:
	VESSEL* owner;
	int anim, transforms;
	double position;
	MGROUP_TRANSFORM **door;
};
