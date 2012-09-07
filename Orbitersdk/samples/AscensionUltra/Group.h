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
#include <vector>
#include <hash_map>
#include <string>

class Group : public GroupElement
{
public:
	Group();
	Group(char *name);
	void Add(GroupElement *element);
	GroupElement * operator [](int index);
	GroupElement * operator [](char *name);
	GroupElement * operator [](char **name);
	virtual GroupElementType GetType() {return TypeGroup;}
	virtual void Displace(VECTOR3 offset);
private:
	std::vector<GroupElement *> byIndex;
	stdext::hash_map<std::string, GroupElement *> byName;
};
