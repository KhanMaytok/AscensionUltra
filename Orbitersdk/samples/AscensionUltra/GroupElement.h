// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// GroupElement.h
// Class definition of base class for group elements.
// ==============================================================

#pragma once
#include "orbitersdk.h"

enum GroupElementType
{
	TypeGroupElement,
	TypeGroup,
	TypeBeaconArray,
	TypeBeaconPath,
};

class GroupElement
{
public:
	GroupElement(void) {name=NULL;}
	~GroupElement(void) {delete [] name;}
	char *GetName() {return name;}
	virtual GroupElementType GetType() {return TypeGroupElement;}
	virtual void Displace(VECTOR3 offset) {}
	virtual void Switch(bool on) {}
protected:
	void SetName(char *name)
	{
		delete [] this->name;
		this->name=NULL;
		if (name==NULL) return;
		this->name = new char[strlen(name)+1];
		strcpy(this->name, name);
	}
private:
	char *name;
};
