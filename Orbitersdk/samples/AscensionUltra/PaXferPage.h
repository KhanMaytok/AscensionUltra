#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)

class PaXferPage: public AscensionTowerPage
{

public:

	PaXferPage(AscensionTowerData *data):AscensionTowerPage(data){}

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
		
		mnu[0].line1="Select room";
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

	char *GetSubtitle(){return "Select Destination for PAX";}

	int GetListSize(){return ((Hangar *)dataRoot)->GetRooms();}

	AscensionTowerListPair GetListItem(int index)
	{
		static char text[57];
		AscensionTowerListPair item=
		{
			index,
			text
		};
		Room *r=((Hangar *)dataRoot)->GetRoom(index);
		VESSEL *v=r->GetDock();
		sprintf(text, "%c %s", v==vessel?'*':v==NULL?' ':'o', r->GetName());
		return item;		
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		Room *r=((Hangar *)dataRoot)->GetRoom(selectedIndex);
		VESSEL *v=r->GetDock();
		if (v==NULL) ascension->DockVessel(r, vessel);
		else if (v==vessel) ascension->DockVessel(r, NULL);
		return NoChange;
	}

};