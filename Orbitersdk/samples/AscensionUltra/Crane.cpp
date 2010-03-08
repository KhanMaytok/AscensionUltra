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

Crane::Crane(VESSEL *owner, MGROUP_TRANSLATE *X, MGROUP_TRANSLATE *Y, MGROUP_TRANSLATE *Z)
{
	this->owner=owner;
	mgroupX=X;
	mgroupY=Y;
	mgroupZ=Z;	
}
Crane::~Crane()
{
}
void Crane::SetSpeed(VECTOR3 speed)
{
}
void Crane::SetCrawl(VECTOR3 speed)
{
}
void Crane::SetMargin(VECTOR3 margin)
{
}
void Crane::SetAutoOverride(double percentage)
{
}
void Crane::StartAuto(int list)
{
}
void Crane::Stop()
{
}
void Crane::StartManual()
{
}
void Crane::Teach(int waypoint)
{
}
VECTOR3 Crane::GetPosition()
{
	return _V(0,0,0);
}
void Crane::SetPosition(VECTOR3 position)
{
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
	owner->AddAnimationComponent (anim_x, 0, 1, mgroupX);
	anim_y = owner->CreateAnimation (0);
	owner->AddAnimationComponent (anim_y, 0, 1, mgroupY);
	anim_z = owner->CreateAnimation (0);
	owner->AddAnimationComponent (anim_z, 0, 1, mgroupZ);
}