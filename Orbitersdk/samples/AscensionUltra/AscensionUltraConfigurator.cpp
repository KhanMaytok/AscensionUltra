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