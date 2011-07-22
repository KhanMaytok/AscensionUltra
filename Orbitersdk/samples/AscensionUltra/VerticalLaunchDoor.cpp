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

void VerticalLaunchDoor::DefineAnimations()
{	
	int i=0;
	anim = owner->CreateAnimation (0);
	ANIMATIONCOMPONENT_HANDLE parent = owner->AddAnimationComponent (anim, 0, 0.4, door[i++]);
	parent = owner->AddAnimationComponent (anim, 0.6, 1, door[i++], parent);
	for(;i<transforms;i++) owner->AddAnimationComponent (anim, 0.4, 0.6, door[i], parent);
}