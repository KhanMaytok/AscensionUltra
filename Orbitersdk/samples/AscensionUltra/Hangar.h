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
#include "Room.h"
#include "UMmuSDK.h"

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
	virtual void Init(VESSEL* owner, const char *name, UINT meshIndex, const char *event_prefix);
	virtual int GetDoors();
	virtual Door *GetDoor(int index);
	virtual bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);
	virtual char *GetName();
	virtual int GetRooms();
	virtual Room *GetRoom(int index);
	virtual VECTOR3 GetPosition();
protected:
	VESSEL* owner;
	UINT meshIndex;
	char *event_prefix, *name;
	int cur_door;
};
