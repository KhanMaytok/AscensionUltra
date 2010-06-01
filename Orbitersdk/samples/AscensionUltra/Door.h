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

class Door
{
public:
	~Door(void);
	void Init(VESSEL *owner, const char *name, MGROUP_TRANSFORM *door, const char *event_prefix);
	void SetSpeed(double speed);
	void Stop();
	void Open();
	void Close();
	double GetPosition();
	void PostStep (double simt, double simdt, double mjd);
	void DefineAnimations();
	bool clbkLoadStateEx (char *line);
	void clbkSaveState (FILEHANDLE scn);
	void clbkPostCreation ();
	bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);
	virtual char *GetName();

private:
	void RecordEvent(double command);
	VESSEL* owner;
	MGROUP_TRANSFORM *door;
	int anim;
	double speed;
	double command;
	double position;
	char *event_prefix, *name;
};
