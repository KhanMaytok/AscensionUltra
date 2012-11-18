#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)
#define HANGARS HANGARTYPETA | HANGARTYPELFMC | HANGARTYPEPORT

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

	int GetListSize(){return ascension->GetNearestHangar(HANGARS, vessel)==NULL?4:5;}

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
			case 3: return PassengerTerminal;
			case 4:
				data->GetPage(PassengerTransfer)->SetDataRoot(ascension->GetNearestHangar(HANGARS, vessel));
				return PassengerTransfer;
			default: return Undefined;
		}
	}

};