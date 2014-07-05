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

#pragma once
#include "orbitersdk.h"

class AscensionUltraConfig: public LaunchpadItem
{
public:
	AscensionUltraConfig(): LaunchpadItem() {}
	static BOOL CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

	//LaunchpadItem members
	char *Name();
	char *Description();
	bool clbkOpen (HWND hLaunchpad);
};