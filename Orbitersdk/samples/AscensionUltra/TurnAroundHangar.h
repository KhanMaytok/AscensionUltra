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

const double AIRLOCK_OPERATING_SPEED = 0.1; // Opening/closing speed of outer airlock (1/sec) => cycle = 10 sec

#define CRANEXOFFSET 130.0
#define CRANEYOFFSET 18.0
#define CRANEREELUPPERPOINT 31.0
#define CRANEREELLOWERPOINT 29.0
#define CRANEREELHEIGHT (CRANEREELUPPERPOINT-CRANEREELLOWERPOINT)

class TurnAroundHangar
{
public:
	TurnAroundHangar(void);
	~TurnAroundHangar(void);	
	void DefineAnimations ();
	void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkLoadStateEx (char *line);
	void clbkSaveState (FILEHANDLE scn);
	void clbkPostCreation ();
	void Init(VESSEL* owner, UINT meshIndex, const char *event_prefix);
	Crane *GetCrane();
	Door *GetDoor(int index);
	bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);
private:
	VESSEL* owner;
	UINT meshIndex;
	Crane crane1;
	Door doors[4];
	char *event_prefix;
	int cur_crane, cur_door;
};
