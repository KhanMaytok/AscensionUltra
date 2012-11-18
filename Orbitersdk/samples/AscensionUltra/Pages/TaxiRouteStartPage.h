#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)

class TaxiRouteStartPage: public AscensionTowerPage
{

public:

	TaxiRouteStartPage(AscensionTowerData *data):AscensionTowerPage(data){}

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
		
		mnu[0].line1="Select route start";
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

	char *GetSubtitle(){return "Select Taxi Route Start";}

	int GetListSize(){return ascension->GetTaxiways()->GetPoints();}

	AscensionTowerListPair GetListItem(int index)
	{
		static char text[57];
		Routes *t=ascension->GetTaxiways();
		AscensionTowerListPair item=
		{
			index,
			t->GetPoint(index)
		};		
		sprintf(text, "%c %s", t->Strobing(item.Name)?'*':' ', item.Name);
		item.Name=text;
		return item;
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		data->GetPage(TaxiRouteEndSelection)->SetDataRoot(ascension->GetTaxiways()->GetPoint(selectedIndex));
		return TaxiRouteEndSelection;
	}

};