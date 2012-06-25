// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// Tracker.cpp
// Class implementation of tracker controller.
// ==============================================================

#include "Tracker.h"
#include "Module.h"

void Tracker::Init(VESSEL *owner, const char *name, MGROUP_ROTATE *azimuth, MGROUP_ROTATE *elevation, double rotationOffset, const char *classname, int instance)
{
	this->owner=owner;
	int i=strlen(classname);
	strcpy(this->classname=new char[i+1], classname);
	this->event_prefix=new char[i+40];
	this->instancename=new char[i+40];
	if (instance<0)
	{
		sprintf(this->event_prefix, "%sTGT", classname);
		instancename=NULL;
	}
	else
	{
		sprintf(this->event_prefix, "%s%dTGT", classname, instance);
		sprintf(this->instancename, "%s%d", classname, instance);
	}
	strcpy(this->name=new char[strlen(name)+1], name);
	mgroupAzimuth=azimuth;
	mgroupElevation=elevation;
	this->rotationOffset=rotationOffset;
	target=NULL;
}

Tracker::~Tracker(void)
{
	for(std::vector<BeaconArray *>::iterator i=beacons.begin();i!=beacons.end();i++) delete *i;
	delete [] classname;
	delete [] event_prefix;
	delete [] name;
}

void Tracker::SetTarget(OBJHANDLE target)
{
	this->target=target;
	if (target!=NULL)
	{
		char cbuf[256];
		oapiGetObjectName(target, cbuf, 256);
		owner->RecordEvent(event_prefix, cbuf);
	}
	else owner->RecordEvent(event_prefix, "");
}

OBJHANDLE Tracker::GetTarget(){return target;}

double Tracker::GetSpeed(){return 0;}

double Tracker::GetDistance(){return 0;}

void Tracker::clbkPostStep (double simt, double simdt, double mjd)
{
	if (target!=NULL)
	{
		VECTOR3 pos, loc;
		oapiGetGlobalPos(target, &pos);
		owner->Global2Local(pos, loc);
		loc-=position;
		normalise(loc);

		double azimuth=90*RAD; //Default for gimbal lock position
		if (loc.x<-1E-6 || loc.x>1E-6) azimuth=atan(loc.z/loc.x); //If not locked, calculate arctan
		//Check quadrants
		if (azimuth>0 && loc.z<0) azimuth+=PI;
		if (azimuth<0 && loc.z<0) azimuth+=2*PI;
		if (azimuth<0 && loc.z>0) azimuth+=PI;

		azimuth+=rotationOffset;
		if (azimuth<0) azimuth+=2*PI;
		if (azimuth>2*PI) azimuth-=2*PI;

		double elevator=asin(loc.y);
		if (elevator<0) elevator=0;

		owner->SetAnimation (anim_azimuth, azimuth/2/PI);
		owner->SetAnimation (anim_elevation, elevator*2/PI);
	}
}

void Tracker::clbkPostCreation ()
{	
	owner->SetAnimation (anim_azimuth, 0);
	owner->SetAnimation (anim_elevation, 0);
}

void Tracker::DefineAnimations()
{	
	ReadBeaconDefinition(beacons, classname, position, owner);
	if (instancename!=NULL) ReadBeaconDefinition(beacons, instancename, position, owner);
	anim_azimuth = owner->CreateAnimation (0);
	ANIMATIONCOMPONENT_HANDLE parent = owner->AddAnimationComponent (anim_azimuth, 0, 1, mgroupAzimuth);
	anim_elevation = owner->CreateAnimation (0);
	owner->AddAnimationComponent (anim_elevation, 0, 1, mgroupElevation, parent);
}

bool Tracker::clbkLoadStateEx (char *line)
{
    if (!strnicmp (line, "TGT", 3))
	{
		target=oapiGetObjectByName(line+4);
		return true;
	}
	return false;
}

void Tracker::clbkSaveState (FILEHANDLE scn)
{
	if (target!=NULL)
	{
		char cbuf[256];
		oapiGetObjectName(target, cbuf, 256);
		oapiWriteScenario_string (scn, "\t\tTGT", cbuf);
	}
}

bool Tracker::clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event)
{
	if (!strnicmp (event_type, "TGT", 3))
	{
		target=oapiGetObjectByName((char *)event);
		return true;
	}
	return false;
}

void Tracker::SetPosition(VECTOR3 position){this->position=position;}

char *Tracker::GetName(){return name;}