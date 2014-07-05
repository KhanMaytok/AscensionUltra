// Copyright (C) 2010-2014 Friedrich Kastner-Masilko
//
// This file is part of the Ascension Ultra Project
//
// Ascension Ultra is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of
// the License, or (at your option) any later version.
//
// Ascension Ultra is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Ascension Ultra. If not, see
// <http://www.gnu.org/licenses/>.

#include "Door.h"
#include "BeaconArray.h"

void Door::Init(VESSEL *owner, const char *name, const char *event_prefix, int transforms, ... )
{
	this->owner=owner;
	sprintf(this->event_prefix=new char[strlen(event_prefix)+4], "%sCMD", event_prefix);
	sprintf(actionText=new char[strlen(name)+MAXACTIONLEN], "Opening %s...", name);
	strcpy(this->name=new char[strlen(name)+1], name);
	this->transforms=transforms;
	door=new MGROUP_TRANSFORM*[transforms];
	va_list doors;
	va_start(doors, transforms);
	for(int i=0;i<transforms;i++) door[i]=va_arg(doors, MGROUP_TRANSFORM*);
	va_end(doors);
	SetSpeed(0.1);
}

Door::Door(void)
{
	owner=NULL;
	event_prefix=NULL;
	actionText=NULL;
	name=NULL;
	transforms=0;
	door=NULL;
	position=0;
	speed=0;
	command=0.0;
	crew=NULL;
	action=0;
	anim=0;
	warn=NULL;
}

Door::~Door(void)
{
	delete [] event_prefix;
	delete [] name;
	delete [] actionText;
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
double Door::GetMovement(){return command;}

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
	if (crew)
	{
		if (position<=0 && actionText[0]!='O') crew->SetActionAreaText(action, (char *)memcpy(actionText, "Open", 4));
		if (position>0 && actionText[0]!='C') crew->SetActionAreaText(action, (char *)memcpy(actionText, "Clos", 4));
	}
	if (warn)
	{
		bool on=((BeaconArray *)(*warn)[0])->On();
		if (position>0 && position<1)
		{
			if (!on) warn->Switch(true);
		}
		else
		{
			if (on) warn->Switch(false);
		}
	}
	if (open)
	{
		bool on=((BeaconArray *)(*open)[0])->On();
		if (position<1)
		{
			if (on) open->Switch(false);
		}
		else
		{
			if (!on) open->Switch(true);
		}
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
    if (!_strnicmp (line, "POS", 3))
	{
		sscanf (line+4, "%lf", &position);
		return true;
	} else if (!_strnicmp (line, "CMD", 3))
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
	if (!_strnicmp (event_type, "CMD", 3))
	{
		sscanf (event, "%lf", &command);
		return true;
	}
	return false;
}

void Door::LinkActionArea(UMMUCREWMANAGMENT *crew, int action, VECTOR3 position, double radius)
{
	this->crew=crew;
	this->action=action;
	crew->DeclareActionArea(action, position, radius, true,	NULL, NULL);
}

void Door::SetStatusBeaconGroups(Group *warn, Group *open)
{
	this->warn=warn;
	this->open=open;
}