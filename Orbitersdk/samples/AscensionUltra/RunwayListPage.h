#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)

class RunwayListPage: public AscensionTowerPage
{

public:

	RunwayListPage(AscensionTowerData *data):AscensionTowerPage(data){}

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
		
		mnu[0].line1="Select runway";
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
			return ATCMenu;
		default:
			return AscensionTowerPage::KeyHandler(key);
		}
	}

	char *GetTitle(){return GetNameSafeTitle("ATC");}

	char *GetSubtitle(){return "Select Runway for Landing";}

	int GetListSize(){return ascension->GetRunways()->GetPoints();}

	AscensionTowerListPair GetListItem(int index)
	{
		static char text[57];
		Routes *t=ascension->GetRunways();
		AscensionTowerListPair item=
		{
			index,
			t->GetPoint(index)
		};		
		sprintf(text, "%c %s", t->On(item.Name)?'*':' ', item.Name);
		item.Name=text;
		return item;
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		ascension->GetRunways()->Activate(selectedIndex);
		return NoChange;
	}

};