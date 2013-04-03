// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// FoldingDoor.h
// Class definition of folding door controller.
// ==============================================================

#pragma once
#include "Door.h"

class FoldingDoor:public Door
{
public:
	void DefineAnimations()
	{
		int i=0;
		anim = owner->CreateAnimation (0);
		while(i<transforms)
		{
			ANIMATIONCOMPONENT_HANDLE parent = owner->AddAnimationComponent (anim, 0, 1, door[i++]);
			if (i<transforms) owner->AddAnimationComponent (anim, 0, 1, door[i++], parent);			
		}
	}
};
