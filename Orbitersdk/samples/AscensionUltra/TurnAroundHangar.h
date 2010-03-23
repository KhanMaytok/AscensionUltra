#pragma once

#include "Crane.h"

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
	enum DoorStatus { DOOR_CLOSED, DOOR_OPEN, DOOR_CLOSING, DOOR_OPENING } doors_status;
	double doors_proc;
	void DefineAnimations ();
	void ActivateOuterAirlock (DoorStatus action);
	void RevertOuterAirlock ();	
	void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkLoadStateEx (char *line);
	void clbkSaveState (FILEHANDLE scn);
	void clbkPostCreation ();
	void Init(VESSEL* owner, UINT meshIndex, const char *event_prefix);
	Crane *GetCrane();
	bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);
private:
	VESSEL* owner;
	UINT meshIndex;
	Crane crane1;	
	UINT anim_doors;

	MGROUP_ROTATE *door1, *door2;
	MGROUP_TRANSLATE *door3, *door4, *craneX, *craneY, *craneZ;
	MGROUP_SCALE *craneReel;
	char *event_prefix;
};
