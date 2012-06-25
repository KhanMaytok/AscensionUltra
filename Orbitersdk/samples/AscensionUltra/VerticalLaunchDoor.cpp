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
}

//VerticalLaunchDoor uses implicit knowledge of transformation order to establish multi-sequence animation.
//Transformation 0 is translation of cover group to stand in animation state 0-0.4,
//transformation 1 is translation of cover from stand to VAB in animation state 0.6-1 and
//the rest is rotational movement of clamps in animation state 0.4-0.6.

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
	double positionRoot=min(position/0.4, 1);
	double positionCover=max((position-0.6)/0.4, 0);
	int i=0;
	if (i>=beaconTransforms) return;
	MGROUP_TRANSLATE *t=(MGROUP_TRANSLATE *)beaconTransform[i++];
	char **beaconGrp=(char **)t->grp;
	for(int j=0;j<t->ngrp;j++) (*beaconGroups)[beaconGrp[j]]->Displace(t->shift*positionRoot);
	if (i>=beaconTransforms) return;
	MGROUP_TRANSLATE *t2=(MGROUP_TRANSLATE *)beaconTransform[i++];
	beaconGrp=(char **)t2->grp;
	for(int j=0;j<t2->ngrp;j++) (*beaconGroups)[beaconGrp[j]]->Displace(t->shift*positionRoot+t2->shift*positionCover);
}