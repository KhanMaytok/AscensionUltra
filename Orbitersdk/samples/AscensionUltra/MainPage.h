#pragma once
#pragma warning(disable : 4482)
#include "AscensionTowerPage.h"

class MainPage: public AscensionTowerPage
{
public:
	MainPage(AscensionTowerData *data):AscensionTowerPage(data){}
protected:
	char *LabelRenderer (int bt);
	int MenuRenderer (MFDBUTTONMENU *mnu);
	AscensionTowerPageInstance KeyHandler(DWORD key);
	char *GetTitle(){return "Ascension Tower";}
	char *GetSubtitle(){return "Select request";}
	int GetListSize(){return 5;}
	AscensionTowerListPair GetListItem(int index){static AscensionTowerListPair menu[5]=
	{	{0," Ground Operations"},
		{1," Air Traffic Control"},
		{2," Control Rooms"},
		{3, " Roster"},
		{4, " Reset to Defaults"}};return menu[index];}
	AscensionTowerPageInstance Select(int index=-1);
};

AscensionTowerPageInstance MainPage::Select(int index)
{
	AscensionTowerPage::Select(index);
	switch(selection)
	{
		case 0: return GroundMenu;
		case 1: return ATCMenu;
		case 2: return HangarForRoomSelection;
		case 3: return Roster;
		case 4: return Reset;
		default: return Undefined;
	}
}

// Return button labels
char *MainPage::LabelRenderer (int bt)
{
	switch (bt)
	{
		case 6: return "BAS";
		default: return AscensionTowerPage::LabelRenderer(bt);
	}
}


// Return button menus
int MainPage::MenuRenderer (MFDBUTTONMENU *mnu)
{	
	static MFDBUTTONMENU menu = {"Scan for bases", NULL, 'H'};
	mnu[0].line1="Select request";
	mnu[6]=menu;
	return AscensionTowerPage::MenuRenderer(mnu);
}

// Handling shortcut keys
AscensionTowerPageInstance MainPage::KeyHandler(DWORD key)
{	
	switch(key)
	{	
	case OAPI_KEY_H://Scan for bases
		return BaseSelect;
	default:
		return AscensionTowerPage::KeyHandler(key);
	}
}