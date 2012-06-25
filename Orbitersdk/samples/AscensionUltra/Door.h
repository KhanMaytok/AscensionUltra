// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// Door.h
// Class definition of door controller.
// ==============================================================

#pragma once
#include "orbitersdk.h"
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
	virtual void PostStep (double simt, double simdt, double mjd);
	virtual void DefineAnimations();
	bool clbkLoadStateEx (char *line);
	void clbkSaveState (FILEHANDLE scn);
	void clbkPostCreation ();
	bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);
	void LinkActionArea(UMMUCREWMANAGMENT *crew, int action, VECTOR3 position, double radius);
	virtual char *GetName();

private:
	void RecordEvent(double command);
	int action;
	double speed;
	double command;
	char *event_prefix, *name, *actionText;
	UMMUCREWMANAGMENT *crew;
protected:
	VESSEL* owner;
	int anim, transforms;
	double position;
	MGROUP_TRANSFORM **door;
};
