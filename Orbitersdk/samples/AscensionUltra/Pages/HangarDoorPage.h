#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)
#define HDHANGARS HANGARTYPETA | HANGARTYPELL | HANGARTYPELH | HANGARTYPEVLC

class HangarDoorPage: public AscensionTowerPage
{

public:

	HangarDoorPage(AscensionTowerData *data):AscensionTowerPage(data){}

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

	char *GetSubtitle(){return "Select Hangar for Roll-in/Roll-out";}

	int GetListSize(){return ascension->GetHangars(HDHANGARS);}

	AscensionTowerListPair GetListItem(int index)
	{
		AscensionTowerListPair item =
		{
			index,
			ascension->GetHangar(HDHANGARS, index)->GetName()
		};
		return item;
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		data->GetPage(DoorSelection)->SetDataRoot(ascension->GetHangar(HDHANGARS, selectedIndex));
		return DoorSelection;
	}

};