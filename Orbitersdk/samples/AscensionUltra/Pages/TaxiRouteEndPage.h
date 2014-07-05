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

class TaxiRouteEndPage: public AscensionTowerPage
{

public:

	TaxiRouteEndPage(AscensionTowerData *data):AscensionTowerPage(data){}

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
		
		mnu[0].line1="Select route end";
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
			return TaxiRouteStartSelection;
		default:
			return AscensionTowerPage::KeyHandler(key);
		}
	}

	char *GetTitle(){return GetNameSafeTitle("Ground");}

	char *GetSubtitle()
	{
		static char subtitle[57];
		sprintf(subtitle, "Taxi from %s to", (char *)dataRoot);
		return subtitle;
	}

	int GetListSize(){return ascension->GetTaxiways()->GetPoints(false, (char *)dataRoot);}

	AscensionTowerListPair GetListItem(int index)
	{
		static char text[57];
		Routes *t=ascension->GetTaxiways();
		AscensionTowerListPair item=
		{
			index,
			t->GetPoint(index, false, (char *)dataRoot)
		};		
		sprintf(text, "%c %s", t->Strobing((char *)dataRoot, item.Name)?'*':' ', item.Name);
		item.Name=text;
		return item;
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		Routes *t=ascension->GetTaxiways();
		char *start=(char *)dataRoot;
		char *end=t->GetPoint(selectedIndex, false, start);
		if (t->Strobing(start,end)) t->Strobe(start, end, false);
		else
		{
			t->Strobe(false);
			t->Strobe(start, end, true);
			return GroundMenu;
		}
		return NoChange;
	}

};