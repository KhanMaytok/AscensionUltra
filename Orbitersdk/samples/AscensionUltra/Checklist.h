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
#include <map>

class Hangar;
class AscensionUltra;

class Checklist
{
public:
	Checklist(void);
	~Checklist(void);
	virtual int GetType(void);
	virtual int GetState(void);
	virtual bool SetEvent(int event);
	virtual void SetSubject(OBJHANDLE subject);
	virtual OBJHANDLE GetSubject(void);
	virtual void Init(VESSEL *owner, Hangar *hangar, const char *event_prefix, int state=0);
	virtual void PostStep (double simt, double simdt, double mjd);
	virtual Hangar *GetHangar(void);
	bool clbkLoadStateEx (char *line);
	void clbkSaveState (FILEHANDLE scn);
	bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);
	void SetATCText(int fromState, int toState, LPCWSTR text);

private:	
	char *event_prefix;
	int state;
	std::map<int, std::map<int, LPCWSTR>> atc;

protected:
	void RecordEvent(int event);
	void SetState(int state);
	VECTOR3 GetNosePoint();
	AscensionUltra *owner;
	Hangar *hangar;
	OBJHANDLE subject;
};
