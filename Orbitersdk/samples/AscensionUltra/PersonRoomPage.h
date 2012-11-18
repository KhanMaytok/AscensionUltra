#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)

class PersonRoomPage: public AscensionTowerPage
{

public:

	PersonRoomPage(AscensionTowerData *data):AscensionTowerPage(data){}

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
			return HangarForPersonSelection;
		default:
			return AscensionTowerPage::KeyHandler(key);
		}
	}

	char *GetTitle(){return GetNameSafeTitle("Roster");}

	char *GetSubtitle()
	{
		static char subtitle[57];
		sprintf(subtitle, "%s -> %s -> Room Location",
				ascension->GetPerson((int)((void **)dataRoot)[0]).Name,
				((Hangar *)((void **)dataRoot)[1])->GetName());
			return subtitle;
	}

	int GetListSize(){return ((Hangar *)((void **)dataRoot)[1])->GetRooms();}

	AscensionTowerListPair GetListItem(int index)
	{
		static char text[57];
		AscensionTowerListPair item=
		{
			index,
			text
		};
		Room *r=((Hangar *)((void **)dataRoot)[1])->GetRoom(index);
		sprintf(text, "%c %s",
			ascension->GetPerson((int)((void **)dataRoot)[0]).Location==r?'*':' ',
			r->GetName());
		return item;		
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		index=ascension->ChangePerson((int)((void **)dataRoot)[0], PERSON_LOCATION, ((Hangar *)((void **)dataRoot)[1])->GetRoom(selectedIndex));
		if (index<0) return NoChange; //TODO: denial sound
		data->GetPage(Roster)->SetIndex(index);
		return PersonControl;
	}

};