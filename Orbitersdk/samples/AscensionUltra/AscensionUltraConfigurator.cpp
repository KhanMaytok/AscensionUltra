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
#define STRICT
#define ORBITER_MODULE
#include "orbitersdk.h"
#include <stdio.h>
#include <io.h>
#include "AscensionUltraConfigurator.h"
#include "AscensionUltraConfig.h"
#include "RootConfig.h"

#define SECTION		"Settings"
#define SPAWN		"auto-spawn"
#define RESET		"fast-reset"
#define SCNSAVE		"scenario-save"
#define RECSAVE		"recorder-save"
#define INIFILE		"Modules\\AscensionUltra.ini"

gParamsType gParams;

DLLCLBK void InitModule (HINSTANCE hDLL)
{
	gParams.hInst = hDLL;
	gParams.item = new AscensionUltraConfig;	
	gParams.root = NULL;
	
	LAUNCHPADITEM_HANDLE root = oapiFindLaunchpadItem ("Base configuration");
	if (root==NULL)
	{
		gParams.root = new RootConfig;
		root=oapiRegisterLaunchpadItem(gParams.root);
	}
		
	oapiRegisterLaunchpadItem (gParams.item, root);
}

DLLCLBK void ExitModule (HINSTANCE hDLL)
{
	// Unregister the launchpad items
	oapiUnregisterLaunchpadItem (gParams.item);
	delete gParams.item;
	if (gParams.root!=NULL)
	{
		oapiUnregisterLaunchpadItem (gParams.root);	
		delete gParams.root;
	}
}