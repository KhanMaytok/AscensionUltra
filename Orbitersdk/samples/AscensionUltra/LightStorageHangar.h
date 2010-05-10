// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// LightStorageHangar.h
// Class definition of light storage hangar building.
// ==============================================================
#pragma once

#include "Door.h"

class LightStorageHangar
{
public:
	LightStorageHangar(void);
	~LightStorageHangar(void);	
	void DefineAnimations ();
	void clbkPostStep (double simt, double simdt, double mjd);
	bool clbkLoadStateEx (char *line);
	void clbkSaveState (FILEHANDLE scn);
	void clbkPostCreation ();
	void Init(VESSEL* owner, UINT meshIndex, const char *event_prefix);
	Door *GetDoor(int index);
	bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);
private:
	VESSEL* owner;
	UINT meshIndex;
	Door doors[4];
	char *event_prefix;
	int cur_door;
};
