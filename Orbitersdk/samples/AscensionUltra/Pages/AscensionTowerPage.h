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
#include "AscensionTowerData.h"

typedef enum AscensionTowerPageInstance
{
	BaseSelect,
	MainMenu,
	GroundMenu,
	ATCMenu,
	HangarForRoomSelection,
	RoomSelection,
	HangarForDoorSelection,
	DoorSelection,
	TaxiRouteStartSelection,
	TaxiRouteEndSelection,
	HangarForCraneSelection,
	HangarForLaunchSelection,
	LaunchTunnelWizard,
	VerticalLaunchWizard,
	Bearing,
	LandingRunwaySelection,
	LaunchTunnelATC,
	DoorControl,
	CraneControl,
	Roster,
	PersonControl,
	RoomForPersonSelection,
	HangarForPersonSelection,
	PassengerTransfer,
	CraneList,
	CraneGrapple,
	Reset,
	NoChange,
	Undefined,
	CurrentState,
};

struct AscensionTowerListPair
{
	UINT Index;
	char *Name;
};

class AscensionTower;
class AscensionTowerData;

struct AscensionTowerCallbackData
{
	int Flags;
	int Index;
	void *Data;
};

class AscensionTowerPage
{
public:
	AscensionTowerPage(AscensionTowerData *data);
	void Update();
	char *GetButtonLabel (int bt);
	int GetButtonMenu (MFDBUTTONMENU *mnu);
	AscensionTowerPageInstance SetButton(int bt);
	AscensionTowerPageInstance SetKey(DWORD key);
	void SetDataRoot(void *dataRoot); // Sets the root object of the page data items, e.g. the hangar the doors are in on the doors list page
	void SetIndex(int index=-1);
	void RefreshHandles(AscensionUltra *ascension);
	AscensionTowerPageInstance SetEvent(BaseVessel::EventHandler::Arguments args);
	
protected:
	AscensionTowerData *data;
	AscensionUltra *ascension;
	AscensionTower *mfd;
	VESSEL *vessel;
	int selectedIndex, selection, page;
	char *GetNameSafeTitle(char *trailer);
	virtual void MFDRenderer();
	virtual int GetListSize();
	virtual AscensionTowerListPair GetListItem(int index);
	virtual AscensionTowerPageInstance Select(int index=-1);
	virtual char *GetTitle();
	virtual char *GetSubtitle();
	virtual char *LabelRenderer (int bt);
	virtual int MenuRenderer (MFDBUTTONMENU *mnu);
	virtual AscensionTowerPageInstance ButtonHandler(int bt);
	virtual AscensionTowerPageInstance KeyHandler(DWORD key);
	virtual AscensionTowerPageInstance EventHandler(BaseVessel::EventHandler::Arguments args);
	int AT_BUTTON[6];
	int AT_BUTTONDOUBLED[10];
	void *dataRoot;

private:
	
};
