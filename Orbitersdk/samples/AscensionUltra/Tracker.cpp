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

void Tracker::Init(VESSEL *owner, const char *ini, const char *name, MGROUP_ROTATE *azimuth, MGROUP_ROTATE *elevation, double rotationOffset, const char *classname, int instance)
{
	this->owner=owner;
	int i=strlen(classname);
	strcpy(this->classname=new char[i+1], classname);
	this->event_prefix=new char[i+40];
	this->instancename=new char[i+40];
	this->ini=ini;
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

	// ATTENTION: You might need / want to load these values from the config / scenario file.

	VerticalAxisRotationRate = 0.6;		// One revolution in about 10 seconds.
	HorizontalAxisRotationRate = 0.3;		// From looking straight to looking up in about 5 seconds.
	CurrentElevation = 0;			// Load this from scenario, save it when you quit. That way, when the simulation is restarted from the same point, the dish won't spwan at 0, 0 as written in clbkPostCreation, but will actually resume from where it was before the simulation ended.
	CurrentAzimuth = 0;			// Load this from scenario, save it when you quit. 
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

		double DesiredAzimuth = CurrentAzimuth;		// ATTENTION: Changed gimbal lock azimuth to whatever it was the last frame. No need to rotate east for no reason.
		if (loc.x<-1E-6 || loc.x>1E-6) DesiredAzimuth=atan(loc.z/loc.x); //If not locked, calculate arctan
		//Check quadrants
		if (DesiredAzimuth>0 && loc.z<0) DesiredAzimuth+=PI;
		if (DesiredAzimuth<0 && loc.z<0) DesiredAzimuth+=2*PI;
		if (DesiredAzimuth<0 && loc.z>0) DesiredAzimuth+=PI;

		DesiredAzimuth+=rotationOffset;
		if (DesiredAzimuth<0) DesiredAzimuth+=2*PI;
		if (DesiredAzimuth>2*PI) DesiredAzimuth-=2*PI;

		double DesiredElevation=asin(loc.y);
		if (DesiredElevation<0) DesiredElevation=0;

		double AngleChange = HorizontalAxisRotationRate * simdt;
		double AngleDifference = DesiredElevation - CurrentElevation;

		if (AngleChange < abs(AngleDifference))
		{
			// In this simdt, the dish cannot rotate enough to match the elevation of the target. Therefore it is moved a maximum angle.

			int Sign = 1;

			if ((DesiredElevation - CurrentElevation) < 0)
			{
				Sign = -1;
			}

			CurrentElevation = CurrentElevation + Sign * AngleChange;
		}

		else
		{
			// The angle difference is small enough that the dish would catch up in a single frame. 

			CurrentElevation = DesiredElevation;
		}

		AngleChange = VerticalAxisRotationRate * simdt;

		// Calculate the angle difference between current and desired azimuth.

		AngleDifference = CurrentAzimuth - DesiredAzimuth;

		if (DesiredAzimuth > CurrentAzimuth) 
		{
			AngleDifference = DesiredAzimuth - CurrentAzimuth;
		}

		if (AngleDifference > PI)
		{
			AngleDifference = (2 * PI) - AngleDifference;
		}

		if (AngleChange < AngleDifference)
		{
			// Determine the direction the dish has to rotate.

			double Direction = sin(DesiredAzimuth + ((2 * PI) - CurrentAzimuth));	

			if (DesiredAzimuth > CurrentAzimuth)
			{
				Direction = sin(DesiredAzimuth - CurrentAzimuth);
			}

			int Dir = 1;

			if (Direction < 0)
			{
				Dir = -1;
			}

			CurrentAzimuth = CurrentAzimuth + Dir * AngleChange;
		}

		else
		{
			CurrentAzimuth = DesiredAzimuth;
		}

		if (CurrentAzimuth < 0)
		{
			CurrentAzimuth += 2 * PI;
		}

		if (CurrentAzimuth > 2 * PI)
		{
			CurrentAzimuth -= 2 * PI;
		}

		owner->SetAnimation (anim_azimuth, CurrentAzimuth / (2 * PI));
		owner->SetAnimation (anim_elevation, CurrentElevation);
	}
}

void Tracker::clbkPostCreation ()
{	
	owner->SetAnimation (anim_azimuth, 0);
	owner->SetAnimation (anim_elevation, 0);
}

void Tracker::DefineAnimations()
{	
	ReadBeaconDefinition(beacons, ini, classname, position, owner);
	if (instancename!=NULL) ReadBeaconDefinition(beacons, ini, instancename, position, owner);
	anim_azimuth = owner->CreateAnimation (0);
	ANIMATIONCOMPONENT_HANDLE parent = owner->AddAnimationComponent (anim_azimuth, 0, 1, mgroupAzimuth);
	anim_elevation = owner->CreateAnimation (0);
	owner->AddAnimationComponent (anim_elevation, 0, 1, mgroupElevation, parent);
}

bool Tracker::clbkLoadStateEx (char *line)
{
    if (!_strnicmp (line, "TGT", 3))
	{
		// target=oapiGetObjectByName(line+4);	// WARNING: This doesn't work! Don't ask me why...
		// target = oapiGetFocusObject();		// WARNING: This doesn't work either!
		target = owner->GetHandle();		// This works fine, however. 

		if ((target != NULL) && (owner != NULL))
		{	
			// Snap the dish directly to the target when the scenario loads. 

			VECTOR3 pos, loc;
			oapiGetGlobalPos(target, &pos);
			owner->Global2Local(pos, loc);
			loc-=position;
			normalise(loc);

			double DesiredAzimuth = 0;
			if (loc.x<-1E-6 || loc.x>1E-6) DesiredAzimuth=atan(loc.z/loc.x); //If not locked, calculate arctan
			//Check quadrants
			if (DesiredAzimuth>0 && loc.z<0) DesiredAzimuth+=PI;
			if (DesiredAzimuth<0 && loc.z<0) DesiredAzimuth+=2*PI;
			if (DesiredAzimuth<0 && loc.z>0) DesiredAzimuth+=PI;

			DesiredAzimuth+=rotationOffset;
			if (DesiredAzimuth<0) DesiredAzimuth+=2*PI;
			if (DesiredAzimuth>2*PI) DesiredAzimuth-=2*PI;

			double DesiredElevation=asin(loc.y);
			if (DesiredElevation<0) DesiredElevation=0;

			CurrentAzimuth = DesiredAzimuth;
			CurrentElevation = DesiredElevation;

			owner->SetAnimation (anim_azimuth, DesiredAzimuth / (2 * PI));
			owner->SetAnimation (anim_elevation, DesiredElevation);

			return true;
		}
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
	if (!_strnicmp (event_type, "TGT", 3))
	{
		target=oapiGetObjectByName((char *)event);
		return true;
	}
	return false;
}

void Tracker::SetPosition(VECTOR3 position){this->position=position;}

char *Tracker::GetName(){return name;}