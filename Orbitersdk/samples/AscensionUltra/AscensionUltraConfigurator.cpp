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
#include "AscensionUltraSpawner.h"

gParamsType gParams;

DLLCLBK void InitModule (HINSTANCE hDLL)
{
	gParams.hInst = hDLL;
	gParams.Item = new AscensionUltraConfig;	
	gParams.Root = NULL;
	
	LAUNCHPADITEM_HANDLE root = oapiFindLaunchpadItem ("Base configuration");
	if (root==NULL)
	{
		gParams.Root = new RootConfig;
		root=oapiRegisterLaunchpadItem(gParams.Root);
	}

	oapiRegisterLaunchpadItem (gParams.Item, root);

	oapiRegisterModule(new AscensionUltraSpawner(hDLL));
}

DLLCLBK void ExitModule (HINSTANCE hDLL)
{
	// Unregister the launchpad items
	oapiUnregisterLaunchpadItem (gParams.Item);
	delete gParams.Item;
	if (gParams.Root!=NULL)
	{
		oapiUnregisterLaunchpadItem (gParams.Root);	
		delete gParams.Root;
	}
}