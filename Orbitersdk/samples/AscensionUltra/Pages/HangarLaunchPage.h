#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)
#define HANGARS HANGARTYPELFMC | HANGARTYPEVLC

class HangarLaunchPage: public AscensionTowerPage
{

public:

	HangarLaunchPage(AscensionTowerData *data):AscensionTowerPage(data){}

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

	char *GetSubtitle(){return "Select Hangar for Launch";}

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
		Hangar *hangar=ascension->GetHangar(HANGARS, selectedIndex);
		switch(hangar->GetType())
		{
		case HANGARTYPELFMC:
			data->GetPage(LaunchTunnelWizard)->SetDataRoot(hangar);
			return LaunchTunnelWizard;
		case HANGARTYPEVLC:
			data->GetPage(VerticalLaunchWizard)->SetDataRoot(hangar);
			return VerticalLaunchWizard;
		default:
			return NoChange;
		}		
	}

};