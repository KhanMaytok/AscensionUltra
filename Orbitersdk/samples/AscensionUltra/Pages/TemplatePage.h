#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)

class TemplatePage: public AscensionTowerPage
{

public:

	TemplatePage(AscensionTowerData *data):AscensionTowerPage(data){}

protected:

	void MFDRenderer()
	{
		//TODO: put rendering here or remove for default list rendering
	}

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

	AscensionTowerPageInstance ButtonHandler(int bt)
	{
		return AscensionTowerPage::ButtonHandler(bt);
	}

	AscensionTowerPageInstance KeyHandler(DWORD key)
	{	
		switch(key)
		{	
		case OAPI_KEY_H:
			return MainMenu;
		case OAPI_KEY_B:
			/*
				switch(state)
				{
				case AscensionTowerState::PassengerTerminal: SetState(AscensionTowerState::GroundMenu);break;
				case AscensionTowerState::Fueling: SetState(AscensionTowerState::PassengerTerminal);break;
				case AscensionTowerState::LaunchPrepare: SetState(AscensionTowerState::Fueling);break;
				case AscensionTowerState::Bearing: SetState(AscensionTowerState::ATCMenu);break;
				case AscensionTowerState::Launch: SetState(AscensionTowerState::ATCMenu);break;
			*/			
			return MainMenu;
		default:
			return AscensionTowerPage::KeyHandler(key);
		}
	}

	char *GetTitle(){return GetNameSafeTitle("ATC");}

	char *GetSubtitle()
	{
		return "Empty page";
		/*
		case AscensionTowerState::PassengerTerminal: return "Passenger Terminal";	
		case AscensionTowerState::Fueling: return "Fueling";	
		case AscensionTowerState::LaunchPrepare: return "Launch Tunnel";	
		case AscensionTowerState::Bearing: return "Bearing";	
		case AscensionTowerState::Launch: return "Request Launch Clearance";	
		}*/
	}

	int GetListSize()
	{
		return 0;
	}

	AscensionTowerListPair GetListItem(int index)
	{
		static AscensionTowerListPair nullitem={0,NULL};
		return nullitem;
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		return Undefined;
	}

};