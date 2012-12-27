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

class Checklist
{
public:
	Checklist(void);
	~Checklist(void);
	int GetState(void);
	virtual bool SetEvent(int event);
	virtual bool SetSubject(OBJHANDLE subject);
	virtual void Init(VESSEL *owner, const char *event_prefix, int state=0);
	virtual void PostStep (double simt, double simdt, double mjd);
	bool clbkLoadStateEx (char *line);
	void clbkSaveState (FILEHANDLE scn);
	bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);

private:	
	char *event_prefix;

protected:
	void RecordEvent(int event);
	VESSEL* owner;
	OBJHANDLE subject;
	int state;
};
