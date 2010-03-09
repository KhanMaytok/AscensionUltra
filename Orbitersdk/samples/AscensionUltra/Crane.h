// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// Crane.h
// Class definition of crane controller.
// ==============================================================

#pragma once
#include "orbitersdk.h"
#include <vector>

class Crane
{
public:
	void Init(VESSEL *owner, MGROUP_TRANSLATE *X, MGROUP_TRANSLATE *Y, MGROUP_TRANSLATE *Z, MGROUP_SCALE *Reel);
	void SetSpeed(VECTOR3 speed);
	void SetCrawl(VECTOR3 speed);
	void SetMargin(VECTOR3 margin);
	void SetAutoOverride(double percentage);
	void StartAuto(int list);
	void Stop();
	void StartManual();
	void Teach(int waypoint);
	VECTOR3 GetPosition();
	void SetPosition(VECTOR3 position);
	void PostStep (double simt, double simdt, double mjd);
	void DefineAnimations();

private:
	void Crane::SetAnimation (int animation, double &position);
	VESSEL* owner;
	MGROUP_TRANSLATE *mgroupX;
	MGROUP_TRANSLATE *mgroupY;
	MGROUP_TRANSLATE *mgroupZ;
	MGROUP_SCALE *mgroupReel;
	int anim_x, anim_y, anim_z;	
	VECTOR3 speed;
	VECTOR3 crawl;
	VECTOR3 margin;
	VECTOR3 position;
	VECTOR3 command;
	VECTOR3 len;
	double autoOverride;
	std::vector<int> lists;
	std::vector<int> waypoints;
};
