#include "MainPage.h"
#pragma warning(disable : 4482)

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