// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// Crane.cpp
// Class implementation of crane controller.
// ==============================================================

#include "Crane.h"

void Crane::Init(VESSEL *owner, const char *name, MGROUP_TRANSLATE *X, MGROUP_TRANSLATE *Y, MGROUP_TRANSLATE *Z, MGROUP_SCALE *Reel, const char *event_prefix)
{
	this->owner=owner;
	sprintf(this->event_prefix=new char[strlen(event_prefix)+4], "%sCMD", event_prefix);
	strcpy(this->name=new char[strlen(name)+1], name);
	mgroupX=X;
	mgroupY=Y;
	mgroupZ=Z;
	mgroupReel=Reel;
	len=_V(length(mgroupX->shift),length(mgroupY->shift),length(mgroupZ->shift));
	filter=NULL;
	position=_V(0,0,0);
	oldcommand=command=_V(0.0,0.0,0.0);
	for(int i=0;i<WAYPOINTS;i++) waypoints[i]=_V(LISTEMPTY, 0,0);
}

Crane::~Crane(void)
{
	delete filter;
	delete [] event_prefix;
	delete [] name;
}

void Crane::SetSpeed(VECTOR3 speed){this->speed=speed;}
void Crane::SetCrawl(VECTOR3 speed){crawl=speed;}
void Crane::SetMargin(VECTOR3 margin){this->margin=margin;}
void Crane::SetAutoOverride(double percentage){autoOverride=percentage;}

void Crane::StartAuto(int waypoint)
{
}

void Crane::Stop()
{
	if (filter!=NULL)
	{
		sprintf(oapiDebugString(), "Crane offline");
		delete filter;
		filter=NULL;
	}
	RecordEvent(oldcommand=command=_V(0.0,0.0,0.0));
}

void Crane::StartManual()
{
	sprintf(oapiDebugString(), "Crane online (A/D long axis, W/S short axis, Q/E reel, B to return)");
	RecordEvent(oldcommand=command=_V(0.0,0.0,0.0));
	if (filter!=NULL) delete filter;
	filter=new KeyboardFilter(this, &Crane::ConsumeDirectKey, &Crane::Prefilter);
}

VECTOR3 Crane::GetPosition(){return _V(position.x*len.x, position.y*len.y, position.z*len.z);}

VECTOR3 Crane::GetLength(){return len;}

void Crane::SetPosition(VECTOR3 pos)
{
	position=pos;
	SetAnimation(anim_x, position.x/=len.x);
	SetAnimation(anim_y, position.y/=len.y);
	SetAnimation(anim_z, position.z/=len.z);
}

void Crane::PostStep (double simt, double simdt, double mjd)
{
	position+=command*simdt;
	if (command.x!=0.0) SetAnimation(anim_x, position.x);
	if (command.y!=0.0) SetAnimation(anim_y, position.y);
	if (command.z!=0.0) SetAnimation(anim_z, position.z);
}

void Crane::SetAnimation (int animation, double &position)
{
	if (position<0) position=0;
	if (position>1) position=1;
	owner->SetAnimation (animation, position);	
}

void Crane::DefineAnimations()
{	
	anim_x = owner->CreateAnimation (0);
	ANIMATIONCOMPONENT_HANDLE parent = owner->AddAnimationComponent (anim_x, 0, 1, mgroupX);
	anim_y = owner->CreateAnimation (0);
	parent = owner->AddAnimationComponent (anim_y, 0, 1, mgroupY, parent);
	anim_z = owner->CreateAnimation (0);
	owner->AddAnimationComponent (anim_z, 0, 1, mgroupReel, parent);
	owner->AddAnimationComponent (anim_z, 0, 1, mgroupZ, parent);
}

int Crane::ConsumeDirectKey (void *crane, char *kstate)
{
	return((Crane *)crane)->ConsumeDirectKey(kstate);
}

void Crane::Prefilter (void *crane, WPARAM &wparam, LPARAM &lparam)
{
	switch (wparam)
	{
	case VK_SHIFT:
		wparam=VK_LSHIFT;
		break;
	case VK_CONTROL:
		wparam=VK_LCONTROL;
		break;
	}
}

int Crane::ConsumeDirectKey (char *kstate)
{
	oldcommand=command;
	command=_V(0,0,0);
	if (KEYDOWN(kstate, OAPI_KEY_B))
	{
		Stop();
		return 1;
	}
	if (KEYDOWN(kstate, OAPI_KEY_A))
	{
		//X neg
		if (KEYMOD_SHIFT(kstate)) command.x=-crawl.x/len.x;
		else command.x=-speed.x/len.x;		
	}
	if (KEYDOWN(kstate, OAPI_KEY_S))
	{
		//Y neg
		if (KEYMOD_SHIFT(kstate)) command.y=-crawl.y/len.y;
		else command.y=-speed.y/len.y;
	}
	if (KEYDOWN(kstate, OAPI_KEY_E))
	{
		//Z neg
		if (KEYMOD_SHIFT(kstate)) command.z=-crawl.z/len.z;
		else command.z=-speed.z/len.z;
	}
	if (KEYDOWN(kstate, OAPI_KEY_D))
	{
		//X pos
		if (KEYMOD_SHIFT(kstate)) command.x=crawl.x/len.x;
		else command.x=speed.x/len.x;
	}
	if (KEYDOWN(kstate, OAPI_KEY_W))
	{
		//Y pos
		if (KEYMOD_SHIFT(kstate)) command.y=crawl.y/len.y;
		else command.y=speed.y/len.y;
	}
	if (KEYDOWN(kstate, OAPI_KEY_Q) && !KEYMOD_CONTROL(kstate)) //Check for Ctrl+Q, too
	{
		//Z pos
		if (KEYMOD_SHIFT(kstate)) command.z=crawl.z/len.z;
		else command.z=speed.z/len.z;
	}
	if (length(command)!=0)
	{
		if (oldcommand.x!=command.x || oldcommand.y!=command.y || oldcommand.z!=command.z) RecordEvent(command);
		return 1;
	}
	return 0;
}

bool Crane::clbkLoadStateEx (char *line)
{
    if (!strnicmp (line, "POS", 3))
	{		
		sscanf (line+4, "%lf%lf%lf", &position.x, &position.y, &position.z);		
		return true;
	}
	return false;
}

void Crane::clbkSaveState (FILEHANDLE scn)
{
	char cbuf[256];	
	sprintf (cbuf, "%0.4f %0.4f %0.4f", position.x, position.y, position.z);
	oapiWriteScenario_string (scn, "\t\tPOS", cbuf);
}

void Crane::clbkPostCreation ()
{	
	SetAnimation(anim_x, position.x);
	SetAnimation(anim_y, position.y);
	SetAnimation(anim_z, position.z);
}

bool Crane::clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event)
{
	if (!strnicmp (event_type, "CMD", 3))
	{
		sscanf (event, "%lf%lf%lf", &command.x, &command.y, &command.z);
		return true;
	}
	return false;
}

void Crane::RecordEvent(VECTOR3 &command)
{
	static char e[40]="";
	sprintf(e, "%0.4f %0.4f %0.4f", command.x, command.y, command.z);
	owner->RecordEvent(event_prefix, e);
}

char *Crane::GetName(){return name;}

int Crane::GetWaypoints(){return WAYPOINTS;}

VECTOR3 Crane::GetWaypoint(int index){return (index>=0 && index<WAYPOINTS)?waypoints[index]:_V(-1,-1,-1);}

void Crane::SetWaypoint(int index, VECTOR3 waypoint){if (index>=0 && index<WAYPOINTS) waypoints[index]=waypoint;}