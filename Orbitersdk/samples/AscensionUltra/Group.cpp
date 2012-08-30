// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// Group.cpp
// Class implementation of group pattern.
// ==============================================================

#include <string.h>
#include "Group.h"

Group::Group(char *name):GroupElement()
{
	SetName(name);
}

void Group::Add(GroupElement *element)
{
}

GroupElement *Group::GetElement(int index)
{
	return NULL;
}

GroupElement *Group::GetElement(char *name)
{
	return NULL;
}