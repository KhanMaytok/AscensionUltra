// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// TurnAroundHangar.h
// Class definition of turn-around hangar building.
// ==============================================================
#pragma once

#include "Crane.h"
#include "Door.h"
#include "Hangar.h"

const double AIRLOCK_OPERATING_SPEED = 0.1; // Opening/closing speed of outer airlock (1/sec) => cycle = 10 sec

#define DOORS 4
#define ROOMS 2
#define CRANEXOFFSET 130.0
#define CRANEYOFFSET 18.0
#define CRANEREELUPPERPOINT 31.0
#define CRANEREELLOWERPOINT 29.0
#define CRANEREELHEIGHT (CRANEREELUPPERPOINT-CRANEREELLOWERPOINT)
#define AREA_NORTH _V(45,0,-51.5)
#define AREA_SOUTH _V(-45,0,51.5)
#define AREA_EAST _V(-116,0,-15)
#define AREA_WEST _V(116,0,15)
#define ROOM_EAST {_V(-88,22,0),_V(1,0,0),_V(-72,0,-33)}
#define ROOM_WEST {_V(88,22,0),_V(-1,0,0),_V(72,0,-33)}

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
	virtual Crane *GetCrane();
	int GetDoors();
	Door *GetDoor(int index);
	int GetRooms();
	Room *GetRoom(int index);
	bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);
	void SetPosition(VECTOR3 position);
	int InitActionAreas(UMMUCREWMANAGMENT *crew, int index);
	bool ActionAreaActivated(int action);
	bool CheckVincinity(VECTOR3 *pos);
private:
	Crane crane1;
	Door doors[DOORS];
	int cur_crane, cur_room;
	Room rooms[ROOMS];
};