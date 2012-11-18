#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)

class RosterPage: public AscensionTowerPage
{

public:

	RosterPage(AscensionTowerData *data):AscensionTowerPage(data){}

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
		
		mnu[0].line1="Select person";
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

	char *GetTitle(){return GetNameSafeTitle("Roster");}

	char *GetSubtitle(){return "Select Person";}

	int GetListSize(){return ascension->GetPersons();}

	AscensionTowerListPair GetListItem(int index)
	{
		static char text[57];
		AscensionTowerListPair item=
		{
			index,
			text
		};
		Person person=ascension->GetPerson(index);
		if (index>0) sprintf(text, "  %s %s", person.MiscId, person.Name);
		else sprintf(text, "  Add new person...");
		return item;
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		data->GetPage(PersonControl)->SetDataRoot((void *)selectedIndex);
		return PersonControl;
	}

};