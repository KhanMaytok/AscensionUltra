// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// VerticalLaunchDoor.h
// Class implementation of automatic "door" sequence for vertical launch facility MLP and service structure controller.
// ==============================================================

#include "VerticalLaunchDoor.h"

void VerticalLaunchDoor::InitBeaconTransform(Group *group, int transforms, ... )
{
	beaconTransforms=transforms;
	beaconTransform=new MGROUP_TRANSFORM*[transforms];
	va_list beaconTransformings;
	va_start(beaconTransformings, transforms);
	for(int i=0;i<transforms;i++) beaconTransform[i]=va_arg(beaconTransformings, MGROUP_TRANSFORM*);
	va_end(beaconTransformings);
	beaconGroups=group;
}

VerticalLaunchDoor::VerticalLaunchDoor():Door()
{
	beaconTransform=NULL;
	beaconTransforms=0;
	beaconGroups=NULL;
}

VerticalLaunchDoor::~VerticalLaunchDoor(void)
{
	for(int i=0;i<beaconTransforms;i++) delete beaconTransform[i];
	delete [] beaconTransform;
	delete beaconGroups;
}

void VerticalLaunchDoor::DefineAnimations()
{	
	int i=0;
	anim = owner->CreateAnimation (0);
	ANIMATIONCOMPONENT_HANDLE parent = owner->AddAnimationComponent (anim, 0, 0.4, door[i++]);
	parent = owner->AddAnimationComponent (anim, 0.6, 1, door[i++], parent);
	for(;i<transforms;i++) owner->AddAnimationComponent (anim, 0.4, 0.6, door[i], parent);
}

void VerticalLaunchDoor::PostStep (double simt, double simdt, double mjd)
{
	Door::PostStep(simt, simdt, mjd);
	//TODO: displace group according to position transformations
}