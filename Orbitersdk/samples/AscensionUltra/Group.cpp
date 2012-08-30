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
	byIndex.push_back(element);
	char *name=element->GetName();
	if (name==NULL) return;
	std::string n;
	n.assign(name);
	byName[n]=element; //In case of name conflicts, this means that the name always will reference the last definition added
}

GroupElement * Group::operator [] (int index)
{
	if (index<0 || index>byIndex.size()) return NULL;
	return byIndex[index];
}

GroupElement *Group::operator [] (char *name)
{
	std::string n;
	n.assign(name);
	return byName.count(n)>0?byName[n]:NULL;
}

GroupElement *Group::operator [] (char **name)
{	
	if (*name == NULL) return this;
	std::string n;
	n.assign(*name++);
	if (byName.count(n)<=0) return NULL;
	GroupElement *element = byName[n];
	if (element->GetType()==TypeGroup) return (*((Group *)element))[name];
	return *name==NULL?this:NULL;	 
}