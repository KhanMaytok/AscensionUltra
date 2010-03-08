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

Crane::Crane(MGROUP_TRANSLATE *X, MGROUP_TRANSLATE *Y, MGROUP_TRANSLATE *Z, int index)
{
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
}
void Crane::DefineAnimations()
{
}