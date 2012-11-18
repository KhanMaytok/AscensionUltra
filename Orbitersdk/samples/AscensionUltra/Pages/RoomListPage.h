#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)

class RoomListPage: public AscensionTowerPage
{

public:

	RoomListPage(AscensionTowerData *data):AscensionTowerPage(data){}

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
		
		mnu[0].line1="Select room";
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
			return HangarForRoomSelection;
		default:
			return AscensionTowerPage::KeyHandler(key);
		}
	}

	char *GetTitle(){return GetNameSafeTitle("Tower");}

	char *GetSubtitle(){return "Select Control Room";}

	int GetListSize(){return ((Hangar *)dataRoot)->GetRooms();}

	AscensionTowerListPair GetListItem(int index)
	{
		static char text[57];
		AscensionTowerListPair item=
		{
			index,
			text
		};
		Room *r=((Hangar *)dataRoot)->GetRoom(index);
		sprintf(text, "%c %s", ascension->GetControlRoom()==r?'*':' ', r->GetName());
		return item;		
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		ascension->SwitchView(((Hangar *)dataRoot)->GetRoom(selectedIndex));
		return NoChange;
	}

};