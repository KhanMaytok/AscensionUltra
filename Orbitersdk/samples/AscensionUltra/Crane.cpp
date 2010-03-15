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

void Crane::Init(VESSEL *owner, MGROUP_TRANSLATE *X, MGROUP_TRANSLATE *Y, MGROUP_TRANSLATE *Z, MGROUP_SCALE *Reel)
{
	this->owner=owner;
	mgroupX=X;
	mgroupY=Y;
	mgroupZ=Z;
	mgroupReel=Reel;
	len=_V(length(mgroupX->shift),length(mgroupY->shift),length(mgroupZ->shift));
}

void Crane::SetSpeed(VECTOR3 speed){this->speed=speed;}
void Crane::SetCrawl(VECTOR3 speed){crawl=speed;}
void Crane::SetMargin(VECTOR3 margin){this->margin=margin;}
void Crane::SetAutoOverride(double percentage){autoOverride=percentage;}

void Crane::StartAuto(int list)
{
}

void Crane::Stop()
{
	command=_V(-0.1,-0.1,-0.1);
}

void Crane::StartManual()
{
	command=_V(0.1,0.1,0.1);
}

void Crane::Teach(int waypoint)
{
}

VECTOR3 Crane::GetPosition()
{
	return _V(position.x*len.x, position.y*len.y, position.z*len.z);
}

void Crane::SetPosition(VECTOR3 pos)
{
	position=pos;
	SetAnimation(anim_x, position.x/=len.x);
	SetAnimation(anim_y, position.y/=len.y);
	SetAnimation(anim_z, position.z/=len.z);
}

void Crane::PostStep (double simt, double simdt, double mjd)
{
	position+=command*simdt;
	SetAnimation(anim_x, position.x);
	SetAnimation(anim_y, position.y);
	SetAnimation(anim_z, position.z);
}

void Crane::SetAnimation (int animation, double &position)
{
	if (position<0) position=0;
	if (position>1) position=1;
	owner->SetAnimation (animation, position);
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

int Crane::ConsumeDirectKey (char *kstate)
{
	command=_V(0,0,0);
	if (KEYDOWN(kstate, OAPI_KEY_A))
	{
		//X neg
		if (KEYMOD_SHIFT(kstate)) command.x=-crawl.x/len.x;
		else command.x=-speed.x/len.x;		
	}
	if (KEYDOWN(kstate, OAPI_KEY_S))
	{
		//Y neg
		if (KEYMOD_SHIFT(kstate)) command.y=-crawl.y/len.y;
		else command.y=-speed.y/len.y;
	}
	if (KEYDOWN(kstate, OAPI_KEY_E))
	{
		//Z neg
		if (KEYMOD_SHIFT(kstate)) command.z=-crawl.z/len.z;
		else command.z=-speed.z/len.z;
	}
	if (KEYDOWN(kstate, OAPI_KEY_D))
	{
		//X pos
		if (KEYMOD_SHIFT(kstate)) command.x=crawl.x/len.x;
		else command.x=speed.x/len.x;
	}
	if (KEYDOWN(kstate, OAPI_KEY_W))
	{
		//Y pos
		if (KEYMOD_SHIFT(kstate)) command.y=crawl.y/len.y;
		else command.y=speed.y/len.y;
	}
	if (KEYDOWN(kstate, OAPI_KEY_Q))
	{
		//Z pos
		if (KEYMOD_SHIFT(kstate)) command.z=crawl.z/len.z;
		else command.z=speed.z/len.z;
	}
	if (length(command)!=0) return 1;
	return 0;
}