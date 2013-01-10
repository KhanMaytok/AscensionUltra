// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// Checklist.h
// Class definition of a checklist.
// ==============================================================

#pragma once
#include "orbitersdk.h"

class Hangar;

class Checklist
{
public:
	Checklist(void);
	~Checklist(void);
	int GetState(void);
	virtual bool SetEvent(int event);
	virtual bool SetSubject(OBJHANDLE subject);
	OBJHANDLE GetSubject(void);
	virtual void Init(VESSEL *owner, Hangar *hangar, const char *event_prefix, int state=0);
	virtual void PostStep (double simt, double simdt, double mjd);
	virtual Hangar *GetHangar(void);
	bool clbkLoadStateEx (char *line);
	void clbkSaveState (FILEHANDLE scn);
	bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);

private:	
	char *event_prefix;

protected:
	void RecordEvent(int event);
	VESSEL *owner;
	Hangar *hangar;
	OBJHANDLE subject;
	int state;
};
