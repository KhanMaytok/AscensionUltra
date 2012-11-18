#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)
#define HANGARS HANGARTYPETA

class HangarCranePage: public AscensionTowerPage
{

public:

	HangarCranePage(AscensionTowerData *data):AscensionTowerPage(data){}

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

	int GetListSize(){return ascension->GetHangars(HANGARS);}

	AscensionTowerListPair GetListItem(int index)
	{
		AscensionTowerListPair item =
		{
			index,
			ascension->GetHangar(HANGARS, index)->GetName()
		};
		return item;
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		dataRoot = ascension->GetHangar(HANGARS, selectedIndex);
		void *crane=((TurnAroundHangar *)dataRoot)->GetCrane();
		data->GetPage(CraneControl)->SetDataRoot(crane);
		data->GetPage(CraneList)->SetDataRoot(crane);
		data->GetPage(CraneGrapple)->SetDataRoot(crane);
		return CraneGrapple;
	}

};