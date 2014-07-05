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

class DoorPage: public AscensionTowerPage
{

public:

	DoorPage(AscensionTowerData *data):AscensionTowerPage(data){}

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
		
		mnu[0].line1="Select command";
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
			return DoorSelection;
		default:
			return AscensionTowerPage::KeyHandler(key);
		}
	}

	char *GetTitle(){return GetNameSafeTitle("Ground");}

	char *GetSubtitle()
	{
		static char subtitle[57];
		sprintf(subtitle, "%s -> %s",
			(char *)((void **)dataRoot)[0],
			((Door *)((void **)dataRoot)[1])->GetName());
		return subtitle;
	}

	int GetListSize(){return 3;}

	AscensionTowerListPair GetListItem(int index)
	{
		static AscensionTowerListPair doorMenu[3]={{0," Open"},{1," Close"},{2," Stop"}};	
		return doorMenu[index];
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		Door *door=(Door *)((void **)dataRoot)[1];
		switch(selectedIndex)
		{
		case 0: door->Open(); break;
		case 1: door->Close(); break;
		case 2: door->Stop(); break;
		}
		return NoChange;
	}

	void MFDRenderer()
	{
		AscensionTowerPage::MFDRenderer();
		mfd->SetWriteStyle(0,2);
		Door* door=(Door *)((void **)dataRoot)[1];
		if (door->GetPosition()<=0) mfd->Write("Closed", 15);
		else if (door->GetPosition()>=1) mfd->Write("Open", 15);
		else mfd->Write("Moving", 15);
	}

};