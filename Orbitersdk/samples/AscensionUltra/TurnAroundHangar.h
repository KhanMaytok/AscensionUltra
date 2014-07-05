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

#include "Crane.h"
#include "Door.h"
#include "Hangar.h"

const double AIRLOCK_OPERATING_SPEED = 0.1; // Opening/closing speed of outer airlock (1/sec) => cycle = 10 sec

#define TADOORS 4
#define TAROOMS 2
#define CRANEXOFFSET 130.0
#define CRANEYOFFSET 18.0
#define CRANEREELUPPERPOINT 31.0
#define CRANEREELLOWERPOINT 29.0
#define CRANEREELHEIGHT (CRANEREELUPPERPOINT-CRANEREELLOWERPOINT)
#define TAAREA_NORTH _V(45,0,-51.5)
#define TAAREA_SOUTH _V(-45,0,51.5)
#define TAAREA_EAST _V(-116,0,-15)
#define TAAREA_WEST _V(116,0,15)
#define TAROOM_EAST {_V(-88,22,0),_V(1,0,0),_V(-72,0,-33)}
#define TAROOM_WEST {_V(88,22,0),_V(-1,0,0),_V(72,0,-33)}
#define TEXTURE_OFFSET 0.125

class TurnAroundHangar:public Hangar
{
public:
	TurnAroundHangar(void);
	int GetType();
	void DefineAnimations ();
	void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkLoadStateEx (char *line);
	void clbkSaveState (FILEHANDLE scn);
	void clbkPostCreation ();
	void clbkVisualCreated (VISHANDLE vis, int refcount);
	virtual Crane *GetCrane();
	int GetDoors();
	Door *GetDoor(int index);
	int GetRooms();
	Room *GetRoom(int index);
	bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);
	void SetPosition(VECTOR3 position);
	int InitActionAreas(UMMUCREWMANAGMENT *crew, int index);
	bool ActionAreaActivated(int action);
	bool CheckVincinity(VECTOR3 *pos, int index=0);
private:
	Crane crane1;
	Door doors[TADOORS];
	int cur_crane;
	Room rooms[TAROOMS];
};