#pragma once
#include "AscensionTowerPage.h"
#include "../LFMCChecklistStates.h"
#pragma warning(disable : 4482)
#define BUFFERLEN 256
#define RUNWAYEND 4000
#define RUNWAYSTART 275
#define RUNWAYLENGTH (RUNWAYEND-RUNWAYSTART)

class LaunchTunnelATCPage: public AscensionTowerPage
{

public:

	LaunchTunnelATCPage(AscensionTowerData *data):AscensionTowerPage(data){}

protected:

	void MFDRenderer()
	{
		char line[BUFFERLEN];
		VECTOR3 global, local;
		
		mfd->SetWriteStyle(0, 2);
		GetChecklistStates();

		//Reverse transformation into hangar coordinates
		vessel->GetGlobalPos(global);
		ascension->Global2Local(global, local);
		local-=list->GetHangar()->GetPosition();
		local.x=-local.x;

		if (abort)
		{
			mfd->SetWriteStyle(0, 3);
			mfd->Write("press ABT to abort launch", 27);
		}
		mfd->SetWriteStyle(0, 2);
		mfd->Write(GetWizardTitle(), 0);
		mfd->Write(GetWizardSubtitle(), 2);
			
		switch(state)
		{
		case LaunchTunnel::LaunchChecklist::Beacons:
			mfd->Write("Cleared to launch.", 11);
			mfd->Write("Heading  130", 13);
			mfd->Write("Maintain 1000", 15);
			break;
		case LaunchTunnel::LaunchChecklist::Speeding:
			mfd->Write("Cleared to launch.", 11);
			sprintf(line, "Speed  %.3d m/s", (int)vessel->GetAirspeed()); mfd->Write(line, 13);
			sprintf(line, "Runway %.3d %%  ", max(100-(int)((local.x-RUNWAYSTART)*100/RUNWAYLENGTH),0)); mfd->Write(line, 15);
			break;
		case LaunchTunnel::LaunchChecklist::TakeOff:
			mfd->Write("You are cleared to", 10);
			mfd->Write("your destination.", 12);
			sprintf(line, "Altitude %.4d / 2000 m ", (int)local.y); mfd->Write(line, 14);
			sprintf(line, "Distance   %.2d / 10   km", (int)(abs(local.x-RUNWAYEND)/1000));	mfd->Write(line, 16);
			break;
		default:
			mfd->Write("Contact Ground Control", 13);
			break;
		}
	}

	char *LabelRenderer (int bt)
	{
		GetChecklistStates();
		
		switch (bt)
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 9:
			case 10:
			case 11: return "";
			case 6: return "HOM";
			case 7: return "BCK";
			case 8: return abort?"ABT":"";
			default: return NULL;
		}
	}

	int MenuRenderer (MFDBUTTONMENU *mnu)
	{	
		static MFDBUTTONMENU menu[3] = 
		{
			{"Main menu", NULL, 'H'},
			{"Go back", NULL, 'B'},
			{"Abort checklist", "procedure", 'A'},
		};

		GetChecklistStates();
		int k=abort?3:2;		// Without abort button, just copy the first 2 right hand buttons, otherwise 3
		
		for(int i=0;i<k;i++) mnu[6+i]=menu[i];
		return 6+k;
	}

	AscensionTowerPageInstance ButtonHandler(int bt)
	{
		GetChecklistStates();
				
		switch (bt)
		{
			case 6: return SetKey(OAPI_KEY_H);
			case 7: return SetKey(OAPI_KEY_B);
			case 8: return abort?SetKey(OAPI_KEY_A):Undefined;
			default: return Undefined;
		}
	}

	AscensionTowerPageInstance KeyHandler(DWORD key)
	{
		GetChecklistStates();
		
		switch(key)
		{	
		case OAPI_KEY_H: return MainMenu;
		case OAPI_KEY_B: return ATCMenu;
		case OAPI_KEY_A:
			if (!abort) return Undefined;
			list->SetEvent(LaunchTunnel::LaunchChecklist::Abort);
			return ATCMenu;
		default: return Undefined;
		}
	}

	char *GetTitle(){return "";}
	char *GetWizardTitle(){return GetNameSafeTitle("ATC");}

	char *GetSubtitle(){return "";}
	char *GetWizardSubtitle(){return "Launch Phase: Start";}

	AscensionTowerPageInstance EventHandler(BaseVessel::EventHandler::Arguments args)
	{
		if (args.SourceType!=BaseVessel::EventHandler::Checklist) return Undefined;
		Checklist *list=(Checklist *)args.Source;
		Hangar *hangar=list->GetHangar();
		if (hangar->GetType()!=HANGARTYPELFMC) return Undefined;
		//Check event and subject according to list
		OBJHANDLE handle=vessel->GetHandle();
		if (list->GetSubject()!=handle) return Undefined;
		if (args.Event==LaunchTunnel::Step) return NoChange; //Refresh display
		if (args.Event!=LaunchTunnel::Aborted && args.Event!=LaunchTunnel::Left) return Undefined;
		return ATCMenu;
	}

private:	
	LaunchTunnel::LaunchChecklist::State state;
	bool abort;
	Checklist *list;
	
	void GetChecklistStates()
	{
		list=(Checklist *)dataRoot;
		state=(LaunchTunnel::LaunchChecklist::State)list->GetState();
		abort=	state==LaunchTunnel::LaunchChecklist::Beacons ||
				state==LaunchTunnel::LaunchChecklist::Speeding;
	}
};