// ==============================================================
//        ORBITER STARTUP MODULE: ASCENSIONULTRACONFIGURATOR
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// AscensionUltraConfigurator.cpp
// Implementation of Ascension Ultra startup module.
// Contains launchpad items and autoloading tools.
// ==============================================================

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