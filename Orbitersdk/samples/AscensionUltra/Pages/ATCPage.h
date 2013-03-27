#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)

class ATCPage: public AscensionTowerPage
{

public:

	ATCPage(AscensionTowerData *data):AscensionTowerPage(data){}

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
		
		mnu[0].line1="Select request";
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

	char *GetTitle(){return GetNameSafeTitle("ATC");}

	char *GetSubtitle(){return "Select ATC request";}

	int GetListSize(){return GetLaunchList()==NULL?2:3;}

	AscensionTowerListPair GetListItem(int index)
	{
		static AscensionTowerListPair menu[3]={{0," Request Bearing"},{1," Request Clearance to Land"},{2," Request Launch Clearance"}};
		return menu[index];
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		switch(selection)
		{
			case 0: return Bearing;
			case 1: return LandingRunwaySelection;
			case 2:
				{
					Checklist *list=GetLaunchList();
					if (list!=NULL)
					{
						if (list->GetState()==LaunchTunnel::LaunchChecklist::LaunchHold) list->SetEvent(LaunchTunnel::LaunchChecklist::Proceed);
						return LaunchTunnelATC;
					}
				}
				//fall-through
			default: return Undefined;
		}
	}

private:

	Checklist *GetLaunchList()
	{
		for(int i=ascension->GetChecklists(HANGARTYPELFMC | HANGARTYPEVLC, vessel);i-->0;)
		{
			Checklist *list=ascension->GetChecklist(HANGARTYPELFMC | HANGARTYPEVLC, i, vessel);
			if (list->GetType()==LaunchTunnel::Launch)
			{
				data->GetPage(LaunchTunnelATC)->SetDataRoot(list);
				LaunchTunnel::LaunchChecklist::State state=(LaunchTunnel::LaunchChecklist::State)list->GetState();
				return (state==LaunchTunnel::LaunchChecklist::LaunchHold	||
						state==LaunchTunnel::LaunchChecklist::Beacons		||
						state==LaunchTunnel::LaunchChecklist::Speeding		||
						state==LaunchTunnel::LaunchChecklist::TakeOff			) ? list : NULL;
			}
		}
		return NULL;
	}

};