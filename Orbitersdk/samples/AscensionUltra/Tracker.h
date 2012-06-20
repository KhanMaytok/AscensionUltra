// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// Tracker.h
// Class definition of tracker controller.
// ==============================================================

#pragma once
#include "orbitersdk.h"
#include <vector>
#include "BeaconArray.h"

class Tracker
{
public:
	~Tracker(void);
	void Init(VESSEL *owner, const char *name, MGROUP_ROTATE *azimuth, MGROUP_ROTATE *elevation, double rotationOffset, const char *classname, int instance);
	virtual void SetTarget(OBJHANDLE target);
	virtual OBJHANDLE GetTarget();
	virtual double GetDistance();
	virtual double GetSpeed();
	void clbkPostStep (double simt, double simdt, double mjd);
	void clbkPostCreation ();
	void DefineAnimations();
	bool clbkLoadStateEx (char *line);
	void clbkSaveState (FILEHANDLE scn);
	bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);
	void SetPosition(VECTOR3 position);
	virtual char *GetName();

private:
	VESSEL* owner;
	MGROUP_ROTATE *mgroupAzimuth;
	MGROUP_ROTATE *mgroupElevation;
	int anim_azimuth, anim_elevation;
	OBJHANDLE target;
	VECTOR3 position;
	double rotationOffset;
	char *event_prefix, *name, *classname;	
	int instance;
	std::vector<BeaconArray *> beacons;
};