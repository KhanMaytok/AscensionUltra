// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// Hangar.h
// Class definition of abstract hangar building.
// ==============================================================
#pragma once
#include "Door.h"

typedef enum HangarType
{
	TurnAround,
	LightStorage,
};

class Hangar
{
public:
	Hangar(void);
	~Hangar(void);	
	virtual HangarType GetType();
	virtual void DefineAnimations ();
	virtual void clbkPostStep (double simt, double simdt, double mjd);
	virtual bool clbkLoadStateEx (char *line);
	virtual void clbkSaveState (FILEHANDLE scn);
	virtual void clbkPostCreation ();
	virtual void Init(VESSEL* owner, UINT meshIndex, const char *event_prefix);
	virtual int GetDoors();
	virtual Door *GetDoor(int index);
	virtual bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);
protected:
	VESSEL* owner;
	UINT meshIndex;
	char *event_prefix;
	int cur_door;
};
