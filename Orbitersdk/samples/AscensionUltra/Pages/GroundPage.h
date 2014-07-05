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
#define GHANGARS HANGARTYPETA | HANGARTYPELFMC | HANGARTYPEPORT

class GroundPage: public AscensionTowerPage
{

public:

	GroundPage(AscensionTowerData *data):AscensionTowerPage(data){}

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
		
		mnu[0].line1="Select request";
		for(int i=0;i<3;i++) mnu[6+i]=menu[i];	
		return AscensionTowerPage::MenuRenderer(mnu);
	}

	AscensionTowerPageInstance KeyHandler(DWORD key)
	{	
		switch(key)
		{	
		case OAPI_KEY_H:
		case OAPI_KEY_B:
			return MainMenu;
		default:
			return AscensionTowerPage::KeyHandler(key);
		}
	}

	char *GetTitle(){return GetNameSafeTitle("Tower");}

	char *GetSubtitle(){return "Select request";}

	int GetListSize(){return vessel==ascension?3:(ascension->GetNearestHangar(GHANGARS, vessel)==NULL?4:5);}

	AscensionTowerListPair GetListItem(int index)
	{
		static AscensionTowerListPair menu[5]=
		{
			{0," Request Roll-in/Roll-out"},
			{1," Request Taxi"},
			{2," Request Cargo Control"},
			{3," Request Launch"},
			{4," Request Passenger Transfer"}
		};
		return menu[index];
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		switch(selection)
		{
			case 0: return HangarForDoorSelection;
			case 1: return TaxiRouteStartSelection;
			case 2: return HangarForCraneSelection;
			case 3:
				{
					Checklist *list=ascension->GetChecklist(HANGARTYPELFMC, 0, vessel);
					if (list!=NULL)
					{
						data->GetPage(LaunchTunnelWizard)->SetDataRoot(list->GetHangar());
						return LaunchTunnelWizard;
					}
					list=ascension->GetChecklist(HANGARTYPEVLC, 0, vessel);
					if (list!=NULL)
					{
						data->GetPage(VerticalLaunchWizard)->SetDataRoot(list->GetHangar());
						return VerticalLaunchWizard;
					}
				}
				return HangarForLaunchSelection;
			case 4:
				data->GetPage(PassengerTransfer)->SetDataRoot(ascension->GetNearestHangar(GHANGARS, vessel));
				return PassengerTransfer;
			default: return Undefined;
		}
	}

};