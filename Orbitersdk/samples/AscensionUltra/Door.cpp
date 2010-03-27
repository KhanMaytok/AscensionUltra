// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// Door.cpp
// Class implementation of door controller.
// ==============================================================

#include "Door.h"

void Door::Init(VESSEL *owner, MGROUP_TRANSFORM *door, const char *event_prefix)
{
	this->owner=owner;
	sprintf(this->event_prefix=new char[strlen(event_prefix)+4], "%sCMD", event_prefix);
	this->door=door;
	SetSpeed(0.1);
	position=0;
	command=0.0;
}

void Door::SetSpeed(double speed){this->speed=speed;}
void Door::Stop(){RecordEvent(command=0.0);}
void Door::Open(){RecordEvent(command=speed);}
void Door::Close(){RecordEvent(command=-speed);}

void Door::RecordEvent(double command)
{
	static char e[8]="";
	sprintf(e, "%0.4f", command);
	owner->RecordEvent(event_prefix, e);
}

double Door::GetPosition(){return position;}

void Door::PostStep (double simt, double simdt, double mjd)
{
	position+=command*simdt;
	if (position<0)
	{
		position=0;
		Stop();
	}
	if (position>1)
	{
		position=1;
		Stop();
	}
	owner->SetAnimation (anim, position);
}

void Door::DefineAnimations()
{	
	anim = owner->CreateAnimation (0);
	owner->AddAnimationComponent (anim, 0, 1, door);
}

bool Door::clbkLoadStateEx (char *line)
{
    if (!strnicmp (line, "POS", 3))
	{
		sscanf (line+4, "%lf", &position);
		return true;
	} else if (!strnicmp (line, "CMD", 3))
	{
		sscanf (line+4, "%lf", &command);
		return true;
	}
	return false;
}

void Door::clbkSaveState (FILEHANDLE scn)
{
	char cbuf[256];	
	sprintf (cbuf, "%0.4f", position);
	oapiWriteScenario_string (scn, "POS", cbuf);
	sprintf (cbuf, "%0.4f", command);
	oapiWriteScenario_string (scn, "CMD", cbuf);
}

void Door::clbkPostCreation ()
{	
	if (position<0)
	{
		position=0;
		Stop();
	}
	if (position>1)
	{
		position=1;
		Stop();
	}
	owner->SetAnimation (anim, position);
}

bool Door::clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event)
{
	if (!strnicmp (event_type, "CMD", 3))
	{
		sscanf (event, "%lf", &command);
		return true;
	}
	return false;
}