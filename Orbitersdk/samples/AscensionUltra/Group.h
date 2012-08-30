// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// Group.h
// Class definition of group pattern.
// ==============================================================

#pragma once
#include "GroupElement.h"

class Group : GroupElement
{
public:
	Group(char *name);
	void Add(GroupElement *element);
	GroupElement *GetElement(int index);
	GroupElement *GetElement(char *name);
	virtual GroupElementType GetType() {return TypeGroup;}
};
