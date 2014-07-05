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
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)
#define HCHANGARS HANGARTYPETA

class HangarCranePage: public AscensionTowerPage
{

public:

	HangarCranePage(AscensionTowerData *data):AscensionTowerPage(data)
	{
		step=1.0;
		start=0;
	}

protected:

	char *LabelRenderer (int bt)
	{
		switch (bt)
		{
			case 6: return "HOM";
			case 7: return "BCK";
			case 8: return "RES";
			default: return AscensionTowerPage::LabelRenderer(bt);
		}
	}

	int MenuRenderer (MFDBUTTONMENU *mnu)
	{	
		static MFDBUTTONMENU menu[3] = 
		{
			{"Main menu", NULL, 'H'},
			{"Go back", NULL, 'B'},
			{"Reset to default", "values", 'R'}
		};
		
		mnu[0].line1="Select hangar";
		for(int i=0;i<3;i++) mnu[6+i]=menu[i];	
		return AscensionTowerPage::MenuRenderer(mnu);		
	}

	AscensionTowerPageInstance KeyHandler(DWORD key)
	{	
		switch(key)
		{	
		case OAPI_KEY_H:
			return MainMenu;
		case OAPI_KEY_B:
			return GroundMenu;
		default:
			return AscensionTowerPage::KeyHandler(key);
		}
	}

	char *GetTitle(){return GetNameSafeTitle("Ground");}

	char *GetSubtitle(){return "Select Hangar for Cargo Crane";}

	int GetListSize(){return ascension->GetHangars(HCHANGARS);}

	AscensionTowerListPair GetListItem(int index)
	{
		AscensionTowerListPair item =
		{
			index,
			ascension->GetHangar(HCHANGARS, index)->GetName()
		};
		return item;
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		dataRoot = ascension->GetHangar(HCHANGARS, selectedIndex);
		void *crane=((TurnAroundHangar *)dataRoot)->GetCrane();
		dataSet[0]=dataRoot;
		dataSet[1]=crane;
		dataSet[2]=&step;
		dataSet[3]=&start;
		data->GetPage(CraneControl)->SetDataRoot(dataSet);
		data->GetPage(CraneList)->SetDataRoot(dataSet);
		data->GetPage(CraneGrapple)->SetDataRoot(dataSet);
		return CraneGrapple;
	}

	void *dataSet[4];
	double step;
	int start;

};