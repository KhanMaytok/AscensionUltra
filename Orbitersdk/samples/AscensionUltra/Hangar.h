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

#define HANGARTYPETA 0x01
#define HANGARTYPELS 0x02
#define HANGARTYPELFMC 0x04
#define HANGARTYPEPORT 0x08

class Hangar
{
public:
	Hangar(void);
	~Hangar(void);	
	virtual int GetType();
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
	virtual int InitActionAreas(UMMUCREWMANAGMENT *crew, int index);
	virtual bool ActionAreaActivated(int action);
	virtual bool CheckVincinity(VECTOR3 *pos);
protected:
	VESSEL* owner;
	UINT meshIndex;
	char *event_prefix, *name;
	int cur_door;
	int areaStart, areaEnd;
	VECTOR3 position;
};
