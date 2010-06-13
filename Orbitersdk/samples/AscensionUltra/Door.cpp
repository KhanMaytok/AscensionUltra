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

void Door::Init(VESSEL *owner, const char *name, const char *event_prefix, int transforms, ... )
{
	this->owner=owner;
	sprintf(this->event_prefix=new char[strlen(event_prefix)+4], "%sCMD", event_prefix);
	strcpy(this->name=new char[strlen(name)+1], name);	
	this->transforms=transforms;
	door=new MGROUP_TRANSFORM*[transforms];
	va_list doors;
	va_start(doors, transforms);
	for(int i=0;i<transforms;i++) door[i]=va_arg(doors, MGROUP_TRANSFORM*);
	va_end(doors);
	SetSpeed(0.1);
	position=0;
	command=0.0;
}

Door::~Door(void)
{
	delete [] event_prefix;
	delete [] name;
	for(int i=0;i<transforms;i++) delete door[i];
	delete [] door;
}

char *Door::GetName(){return name;}

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
	for(int i=0;i<transforms;i++) owner->AddAnimationComponent (anim, 0, 1, door[i]);
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
	oapiWriteScenario_string (scn, "\t\tPOS", cbuf);
	sprintf (cbuf, "%0.4f", command);
	oapiWriteScenario_string (scn, "\t\tCMD", cbuf);
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