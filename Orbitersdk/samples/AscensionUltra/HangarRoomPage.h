#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)

class HangarRoomPage: public AscensionTowerPage
{

public:

	HangarRoomPage(AscensionTowerData *data):AscensionTowerPage(data){}

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
		case OAPI_KEY_B:
			return MainMenu;
		default:
			return AscensionTowerPage::KeyHandler(key);
		}
	}

	char *GetTitle(){return GetNameSafeTitle("Tower");}

	char *GetSubtitle(){return "Select Hangar for Control Room";}

	int GetListSize(){return ascension->GetHangars(HANGARTYPETA | HANGARTYPELFMC | HANGARTYPEPORT | HANGARTYPEVLC);}

	AscensionTowerListPair GetListItem(int index)
	{
		AscensionTowerListPair item =
		{
			index,
			ascension->GetHangar(HANGARTYPETA | HANGARTYPELL | HANGARTYPELFMC | HANGARTYPELH | HANGARTYPEVLC, index)->GetName()
		};
		return item;
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		data->GetPage(RoomSelection)->SetDataRoot(ascension->GetHangar(HANGARTYPETA | HANGARTYPELFMC | HANGARTYPEPORT | HANGARTYPEVLC, selectedIndex));
		return RoomSelection;
	}

};