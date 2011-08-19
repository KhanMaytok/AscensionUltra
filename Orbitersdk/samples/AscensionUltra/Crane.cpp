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
	waypoint=0;
	running=false;
}

Crane::~Crane(void)
{
	delete filter;
	delete [] event_prefix;
	delete [] name;
}

void Crane::SetSpeed(VECTOR3 speed){this->speed=speed;}
void Crane::SetCrawl(VECTOR3 speed){crawl=speed;}
void Crane::SetAutoOverride(double percentage){autoOverride=percentage;}

void Crane::StartAuto(int waypoint)
{
	running=true;
	positioning=false;
	timer=0;
	this->waypoint=waypoint;
	trajectoryCrawl=min(min(crawl.x, crawl.y), crawl.z);
	trajectorySpeed=min(min(speed.x, speed.y), speed.z);
}

void Crane::Stop()
{
	running=false;
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

VECTOR3 Crane::GetPosition(){return _V(position.x, position.y, position.z);}

VECTOR3 Crane::GetLength(){return len;}

VECTOR3 Crane::GetSpeed(){return speed;}

VECTOR3 Crane::GetCrawl(){return crawl;}

int Crane::GetMode(){return filter!=NULL?CRANEDIRECT:(running?waypoint:CRANEMANUAL);}

void Crane::SetMode(int mode)
{
	int oldmode=GetMode();
	if (oldmode==mode) return;
	if (oldmode!=CRANEMANUAL) Stop();
	switch(mode)
	{
	case CRANEDIRECT:
		StartManual();
		break;
	case CRANEMANUAL:
		break;
	default:
		StartAuto(mode);
		break;
	}	
}

void Crane::SetPosition(VECTOR3 pos)
{
	position=pos;
	SetAnimation(anim_x, position.x, len.x);
	SetAnimation(anim_y, position.y, len.y);
	SetAnimation(anim_z, position.z, len.z);
}

void Crane::PostStep (double simt, double simdt, double mjd)
{
	position+=command*simdt;
	if (!running)
	{
		if (command.x!=0.0) SetAnimation(anim_x, position.x, len.x);
		if (command.y!=0.0) SetAnimation(anim_y, position.y, len.y);
		if (command.z!=0.0) SetAnimation(anim_z, position.z, len.z);
		return;
	}
	
	if (waypoint>=WAYPOINTS || waypoint<0) waypoint=0;
	
	if (positioning)
	{
		double commandlen=length(command);
		if (positioning)
		{
			//Check target distance
			VECTOR3 delta=waypoints[waypoint]-position;			
			double deltalen=length(delta);
			if (commandlen!=0 && deltalen!=0)
			{
				//Overshot detection
				if (dotp(command/commandlen, delta/deltalen)<0) commandlen=deltalen=0;
			}
			else commandlen=deltalen=0;
			if (commandlen==0 && deltalen==0)
			{
				//Target reached
				command=_V(0,0,0);
				position=waypoints[waypoint];
				positioning=false;
				timer=0;
				waypoint++;
			}
			else if (deltalen<trajectoryCrawl) //if there is 1 second crawl time left
			{
				command=(command/commandlen)*trajectoryCrawl;
			}
			SetAnimation(anim_x, position.x, len.x);
			SetAnimation(anim_y, position.y, len.y);
			SetAnimation(anim_z, position.z, len.z);
			
		}		
	}
	else if (timer!=0)
	{
		if ((timer-=simdt)<=0)
		{
			timer=0;
			waypoint++;			
		}
	}
	else if (waypoints[waypoint].x<0)
	{
		switch ((int)waypoints[waypoint].x)
		{
		case LISTJUMP:		waypoint=(int)waypoints[waypoint].y; break;
		case LISTPAUSE:
			timer=waypoints[waypoint].y;
			break;
		case LISTGRAPPLE:	waypoint++; break; //TODO: enter auto grapple code here
		case LISTRELEASE:	waypoint++; break; //TODO: enter auto release code here
		case LISTSPEEDS:
			trajectorySpeed=waypoints[waypoint].y;
			trajectoryCrawl=waypoints[waypoint].z;
			waypoint++;
			break;
		default: Stop(); break; //Illegal, LISTSTOP and LISTEMPTY
		}
	}
	else
	{
		//Check target distance
		VECTOR3 delta=waypoints[waypoint]-position;
		double deltalen=length(delta);
		if (deltalen!=0)
		{
			command=(delta/deltalen)*trajectoryCrawl;
			timer=1; // 1 second crawl time
			positioning=true;
		}
		else waypoint++; //Nothing to do, go on			
	}
}

void Crane::SetAnimation (int animation, double &position, double length)
{
	if (position<0) position=0;
	if (position>length) position=length;
	owner->SetAnimation (animation, position/length);	
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
		if (KEYMOD_SHIFT(kstate)) command.x=-crawl.x;
		else command.x=-speed.x;		
	}
	if (KEYDOWN(kstate, OAPI_KEY_S))
	{
		//Y neg
		if (KEYMOD_SHIFT(kstate)) command.y=-crawl.y;
		else command.y=-speed.y;
	}
	if (KEYDOWN(kstate, OAPI_KEY_E))
	{
		//Z neg
		if (KEYMOD_SHIFT(kstate)) command.z=-crawl.z;
		else command.z=-speed.z;
	}
	if (KEYDOWN(kstate, OAPI_KEY_D))
	{
		//X pos
		if (KEYMOD_SHIFT(kstate)) command.x=crawl.x;
		else command.x=speed.x;
	}
	if (KEYDOWN(kstate, OAPI_KEY_W))
	{
		//Y pos
		if (KEYMOD_SHIFT(kstate)) command.y=crawl.y;
		else command.y=speed.y;
	}
	if (KEYDOWN(kstate, OAPI_KEY_Q) && !KEYMOD_CONTROL(kstate)) //Check for Ctrl+Q, too
	{
		//Z pos
		if (KEYMOD_SHIFT(kstate)) command.z=crawl.z;
		else command.z=speed.z;
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
	else if (!strnicmp (line, "L", 1))
	{
		int index;
		char cmd[256];
		sscanf (line+2, "%d %s", &index, cmd);
		if (!strnicmp(strupr(cmd), "STOP", 4)) waypoints[index]=_V(LISTSTOP, 0, 0);
		else if (!strnicmp(strupr(cmd), "GRAPPLE", 7)) waypoints[index]=_V(LISTGRAPPLE, 0, 0);
		else if (!strnicmp(strupr(cmd), "RELEASE", 7)) waypoints[index]=_V(LISTRELEASE, 0, 0);
		else if (!strnicmp(strupr(cmd), "JUMP", 4))
		{
			waypoints[index]=_V(LISTJUMP, 0, 0);
			sscanf (line+2, "%d %s %lf", &index, cmd, &waypoints[index].y);			
		}
		else if (!strnicmp(strupr(cmd), "PAUSE", 5))
		{
			waypoints[index]=_V(LISTPAUSE, 0, 0);
			sscanf (line+2, "%d %s %lf", &index, cmd, &waypoints[index].y);			
		}
		else if (!strnicmp(strupr(cmd), "SPEED", 5))
		{
			waypoints[index]=_V(LISTSPEEDS, 0, 0);
			sscanf (line+2, "%d %s %lf %lf", &index, cmd, &waypoints[index].y, &waypoints[index].z);
		}
		else sscanf (line+2, "%d %lf %lf %lf", &index, &waypoints[index].x, &waypoints[index].y, &waypoints[index].z);
		return true;
	}
	return false;
}

void Crane::clbkSaveState (FILEHANDLE scn)
{
	char cbuf[256];	
	sprintf (cbuf, "%0.4f %0.4f %0.4f", position.x, position.y, position.z);
	oapiWriteScenario_string (scn, "\t\tPOS", cbuf);
	for(int i=0;i<WAYPOINTS;i++)
	{
		if (waypoints[i].x<0)
		{
			switch ((int)waypoints[i].x)
			{
			case LISTSTOP:		sprintf(cbuf, "%d STOP", i); break;
			case LISTJUMP:		sprintf(cbuf, "%d JUMP %d", i, (int)waypoints[i].y); break;
			case LISTPAUSE:		sprintf(cbuf, "%d PAUSE %0.4f", i, waypoints[i].y);break;
			case LISTGRAPPLE:	sprintf(cbuf, "%d GRAPPLE", i); break; //TODO: enter auto grapple code here
			case LISTRELEASE:	sprintf(cbuf, "%d RELEASE", i); break; //TODO: enter auto release code here
			case LISTSPEEDS: sprintf(cbuf, "%d SPEED %0.4f %0.4f", i, waypoints[i].y, waypoints[i].z); break;
			default: continue;
			}
		}
		else sprintf(cbuf, "%d %0.4f %0.4f %0.4f", i, waypoints[i].x, waypoints[i].y, waypoints[i].z);
		oapiWriteScenario_string (scn, "\t\tL", cbuf);
	}
}

void Crane::clbkPostCreation ()
{	
	SetAnimation(anim_x, position.x, len.x);
	SetAnimation(anim_y, position.y, len.y);
	SetAnimation(anim_z, position.z, len.z);
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