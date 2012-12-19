// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// DocksHangar.cpp
// Class implementation of dockyard buildings.
// ==============================================================
#include "DocksHangar.h"

int DocksHangar::GetType(){return HANGARTYPEDOCKS;}

void DocksHangar::SetPosition(VECTOR3 position){this->position=position;}