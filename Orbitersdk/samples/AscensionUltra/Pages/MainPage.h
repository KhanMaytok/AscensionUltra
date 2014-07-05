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
#pragma warning(disable : 4482)
#include "AscensionTowerPage.h"

class MainPage: public AscensionTowerPage
{

public:

	MainPage(AscensionTowerData *data):AscensionTowerPage(data){}

protected:

	char *LabelRenderer (int bt)
	{
		switch (bt)
		{
			case 6: return "BAS";
			default: return AscensionTowerPage::LabelRenderer(bt);
		}
	}

	int MenuRenderer (MFDBUTTONMENU *mnu)
	{	
		static MFDBUTTONMENU menu = {"Scan for bases", NULL, 'H'};
		mnu[0].line1="Select request";
		mnu[6]=menu;
		return AscensionTowerPage::MenuRenderer(mnu);
	}

	AscensionTowerPageInstance KeyHandler(DWORD key)
	{	
		switch(key)
		{	
		case OAPI_KEY_H://Scan for bases
			return BaseSelect;
		default:
			return AscensionTowerPage::KeyHandler(key);
		}
	}

	char *GetTitle(){return GetNameSafeTitle("Tower");}

	char *GetSubtitle(){return "Select request";}

	int GetListSize(){return 5;}

	AscensionTowerListPair GetListItem(int index)
	{
		static AscensionTowerListPair menu[5]=
		{
			{0," Ground Operations"},
			{1," Air Traffic Control"},
			{2," Control Rooms"},
			{3, " Roster"},
			{4, " Reset to Defaults"}
		};
		return menu[index];
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		switch(selection)
		{
			case 0: return GroundMenu;
			case 1: return ATCMenu;
			case 2: return HangarForRoomSelection;
			case 3: return Roster;
			case 4: return Reset;
			default: return Undefined;
		}
	}
};