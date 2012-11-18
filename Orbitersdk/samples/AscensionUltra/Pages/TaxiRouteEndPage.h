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