// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// Checklist.cpp
// Class implementation a checklist.
// ==============================================================

#include "Checklist.h"
#include "AscensionUltra.h"

Checklist::Checklist(void)
{
	owner=NULL;
	hangar=NULL;
	event_prefix=NULL;
	state=0;
	subject=NULL;
}

int Checklist::GetType(void){return -1;}
int Checklist::GetState(void){return state;}
Hangar *Checklist::GetHangar(void){return hangar;}

void Checklist::Init(VESSEL *owner, Hangar *hangar, const char *event_prefix, int state)
{
	this->owner=(AscensionUltra *)owner;
	this->hangar=hangar;
	this->state=state;
	sprintf(this->event_prefix=new char[strlen(event_prefix)+6], "%sEVENT", event_prefix);
	subject=NULL;
}

Checklist::~Checklist(void){delete [] event_prefix;}

bool Checklist::SetEvent(int event){return false;}

void Checklist::SetSubject(OBJHANDLE subject){if (this->subject==NULL || subject==NULL) this->subject=subject;}
OBJHANDLE Checklist::GetSubject(void){return subject;}

void Checklist::RecordEvent(int event)
{
	static char e[80]="";
	sprintf(e, "%d", event);
	owner->RecordEvent(event_prefix, e);
}

void Checklist::PostStep (double simt, double simdt, double mjd){}
bool Checklist::clbkLoadStateEx (char *line)
{
	if (!_strnicmp (line, "STATE", 5))
	{
		sscanf (line+6, "%d", &state);
		return true;
	} else if (!_strnicmp (line, "SUBJECT", 7))
	{
		subject=oapiGetObjectByName(line+8);
		return true;
	}
	return false;
}
void Checklist::clbkSaveState (FILEHANDLE scn)
{
	char cbuf[256];	
	sprintf (cbuf, "%d", state);
	oapiWriteScenario_string (scn, "\t\tSTATE", cbuf);
	if (subject!=NULL)
	{
		oapiGetObjectName(subject, cbuf, 256);
		oapiWriteScenario_string (scn, "\t\tSUBJECT", cbuf);
	}
}
bool Checklist::clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event)
{
	if (!_strnicmp (event_type, "EVENT", 5))
	{
		int e=0;
		sscanf (event, "%d", &e);
		return SetEvent(e);
	}
	return false;
}

VECTOR3 Checklist::GetNosePoint()
{
	//Calculate local coordinates of subject nose w.r.t. hangar - nose is half size in front of origin
	VECTOR3 global, local;
	VESSEL *vessel=oapiGetVesselInterface(subject);
	vessel->Local2Global(_V(0,0,vessel->GetSize()), global);
	owner->Global2Local(global, local);
	return local;
}