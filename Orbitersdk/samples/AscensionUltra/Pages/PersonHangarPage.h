#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)
#define PHHANGARS HANGARTYPETA | HANGARTYPELFMC | HANGARTYPEPORT | HANGARTYPEVLC

class PersonHangarPage: public AscensionTowerPage
{

public:

	PersonHangarPage(AscensionTowerData *data):AscensionTowerPage(data){}

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
			return PersonControl;
		default:
			return AscensionTowerPage::KeyHandler(key);
		}
	}

	char *GetTitle(){return GetNameSafeTitle("Roster");}

	char *GetSubtitle(){return "Select Hangar for Control Room";}

	int GetListSize(){return ascension->GetHangars(PHHANGARS);}

	AscensionTowerListPair GetListItem(int index)
	{
		static char text[57];
		AscensionTowerListPair item=
		{
			index,
			text
		};
		Hangar *h=ascension->GetHangar(PHHANGARS, index);
		sprintf(text, "%c %s",
			ascension->GetPerson((int)dataRoot).Location->GetHangar()==h?'*':' ',
					h->GetName());
		return item;
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		dataSet[0]=dataRoot;
		dataSet[1]=ascension->GetHangar(PHHANGARS, selectedIndex);
		data->GetPage(RoomForPersonSelection)->SetDataRoot(&dataSet);
		return RoomForPersonSelection;
	}

private:
	void* dataSet[2];

};