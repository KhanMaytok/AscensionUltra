// Copyright (C) 2010-2014 Friedrich Kastner-Masilko
//
// This file is part of the Ascension Ultra Project
//
// Ascension Ultra is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of
// the License, or (at your option) any later version.
//
// Ascension Ultra is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Ascension Ultra. If not, see
// <http://www.gnu.org/licenses/>.

#include <string.h>
#include "Group.h"

Group::Group():GroupElement(){}

Group::Group(char *name):GroupElement()
{
	SetName(name);
}

void Group::Displace(VECTOR3 offset){for(std::vector<GroupElement *>::iterator i=byIndex.begin();i!=byIndex.end();i++) (*i)->Displace(offset);}
void Group::Switch(bool on){for(std::vector<GroupElement *>::iterator i=byIndex.begin();i!=byIndex.end();i++) (*i)->Switch(on);}

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
	if (index<0 || index>=(int)byIndex.size()) return NULL;
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