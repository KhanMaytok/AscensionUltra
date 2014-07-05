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

#pragma once
#include "orbitersdk.h"
#include <vector>
#include "KeyboardFilter.h"

#define WAYPOINTS 100

#define LISTSTOP	-2
#define LISTJUMP	-3
#define LISTPAUSE	-4
#define LISTGRAPPLE	-5
#define LISTRELEASE	-6
#define LISTSPEEDS	-7
#define LISTEMPTY	-1

#define CRANEDIRECT	-2
#define CRANEMANUAL	-1

class Crane
{
public:
	~Crane(void);
	void Init(VESSEL *owner, const char *name, MGROUP_TRANSLATE *X, MGROUP_TRANSLATE *Y, MGROUP_TRANSLATE *Z, MGROUP_SCALE *Reel, const char *event_prefix);
	virtual void SetSpeed(VECTOR3 speed);
	virtual void SetCrawl(VECTOR3 speed);
	void SetAutoOverride(double percentage);
	void StartAuto(int waypoint);
	void Stop();
	void StartManual();
	virtual int GetMode();
	virtual void SetMode(int mode);
	virtual void SetWaypoint(int index, VECTOR3 waypoint);
	virtual VECTOR3 GetPosition();
	virtual VECTOR3 GetLength();
	virtual VECTOR3 GetSpeed();
	virtual VECTOR3 GetCrawl();
	virtual void SetPosition(VECTOR3 position);
	void PostStep (double simt, double simdt, double mjd);
	int ConsumeDirectKey (char *kstate);
	void DefineAnimations();
	bool clbkLoadStateEx (char *line);
	void clbkSaveState (FILEHANDLE scn);
	void clbkPostCreation ();
	bool clbkPlaybackEvent (double simt, double event_t, const char *event_type, const char *event);
	virtual char *GetName();
	virtual int GetWaypoints();
	virtual VECTOR3 GetWaypoint(int index);

private:
	void RecordEvent(const char *command, VECTOR3 &value);
	void SetAnimation (int animation, double &position, double length);
	VESSEL* owner;
	MGROUP_TRANSLATE *mgroupX;
	MGROUP_TRANSLATE *mgroupY;
	MGROUP_TRANSLATE *mgroupZ;
	MGROUP_SCALE *mgroupReel;
	int anim_x, anim_y, anim_z, waypoint;	
	VECTOR3 speed;
	VECTOR3 crawl;
	VECTOR3 margin;
	VECTOR3 position;
	VECTOR3 command;
	VECTOR3 oldcommand;
	VECTOR3 len;
	double autoOverride, trajectorySpeed, trajectoryCrawl, timer;
	VECTOR3 waypoints[WAYPOINTS];
	KeyboardFilter *filter;
	static int ConsumeDirectKey (void *crane, char *kstate);
	static void Prefilter (void *crane, WPARAM &wparam, LPARAM &lparam);
	char *event_prefix, *name;
	bool running, positioning;
};
