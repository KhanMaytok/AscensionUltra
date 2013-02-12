#pragma once
#include "AscensionTowerPage.h"
#include "../LFMCChecklistStates.h"
#pragma warning(disable : 4482)
#define BUFFERLEN 256

#define MODEPERSONS		0
#define MODEPROPELLANT	1

bool SetTankLevel(void *id, char *str, void *usrdata)
{
	AscensionTowerCallbackData *cp=(AscensionTowerCallbackData *)usrdata;
	VESSEL *vessel=(VESSEL *)cp->Data;
	char *end=str+strlen(str);
	for(;str!=end;str++) if (*str>32) break;
	PROPELLANT_HANDLE propellant=vessel->GetPropellantHandleByIndex(cp->Index);
	double maxmass=vessel->GetPropellantMaxMass(propellant);
	switch (*str)
	{		
	case 0: // Empty fuel tank
	case 'e':
		vessel->SetPropellantMass(propellant, 0);
		return true;
	case 'f': // Fill up fuel tank
		vessel->SetPropellantMass(propellant, maxmass);
		return true;
	case '%': // Set level percentage explicitly
		{
			unsigned int level;
			if (sscanf(++str, "%u", &level)!=1) return false;
			if (level<0 || level>100) return false;
			vessel->SetPropellantMass(propellant, maxmass*((double)level)/100.0);
			
		}
		return true;
	default: // Set level
		{
			double mass;
			if (sscanf(str, "%lf", &mass)!=1) return false;
			if (maxmass<mass) mass=maxmass;
			if (mass<0) mass=0;
			vessel->SetPropellantMass(propellant, mass);
		}
		return true;
	}
}

class LaunchTunnelWizardPage: public AscensionTowerPage
{

public:

	LaunchTunnelWizardPage(AscensionTowerData *data):AscensionTowerPage(data){mode=0;}

protected:

	void MFDRenderer()
	{
		mfd->SetWriteStyle(0, 2);
		switch(GetChecklistStates())
		{
		case 1: //already subject of launch checklist, check state
			switch (launch)
			{
			case LaunchTunnel::LaunchChecklist::AbortOpen: mfd->Write("Launch aborted - clear the area");break;

			case LaunchTunnel::LaunchChecklist::Empty:
			case LaunchTunnel::LaunchChecklist::OpenExit: mfd->Write("Launchway free - wait for clearance");break;

			case LaunchTunnel::LaunchChecklist::Exit: mfd->Write("Clear to enter - taxi to hold");break;

			case LaunchTunnel::LaunchChecklist::CloseExit:
			case LaunchTunnel::LaunchChecklist::DeployShield: mfd->Write("Contact ATC for clearance");break;

			default: mfd->Write("Contact ATC for launch");break;
			}
			break;
		
		case 0: //only subject of prepare checklist, check state
			switch (prepare)
			{
			case LaunchTunnel::PrepareChecklist::AbortOpen: mfd->Write("Boarding aborted - clear the area");break;

			case LaunchTunnel::PrepareChecklist::Empty:
			case LaunchTunnel::PrepareChecklist::OpenEntry: mfd->Write("Gate free - wait for clearance");break;

			case LaunchTunnel::PrepareChecklist::Entry: mfd->Write("Clear to enter - taxi to gate");break;

			case LaunchTunnel::PrepareChecklist::CloseEntry:
			case LaunchTunnel::PrepareChecklist::Occupied:
				AscensionTowerPage::MFDRenderer(); //Simply do default list rendering
				break;
			case LaunchTunnel::PrepareChecklist::Ready:
				mfd->Write("Launchway occupied", 10);
				mfd->Write("Wait for clearance", 11);
				Checklist *list=hangar->GetChecklist(1);
				OBJHANDLE subject=list->GetSubject();
				if (subject==NULL) return;
				mfd->SetWriteStyle(0);					
				mfd->Write(oapiGetVesselInterface(subject)->GetName(), 13, 2);
				LaunchTunnel::LaunchChecklist::State state=(LaunchTunnel::LaunchChecklist::State) list->GetState();
				switch (state)
				{
				case LaunchTunnel::LaunchChecklist::AbortOpen: mfd->Write("issued abort and clears area", 14, 2);break;
				case LaunchTunnel::LaunchChecklist::Empty:
				case LaunchTunnel::LaunchChecklist::OpenExit:
				case LaunchTunnel::LaunchChecklist::Exit: mfd->Write("exits gate area", 14, 2);break;
				case LaunchTunnel::LaunchChecklist::CloseExit:
				case LaunchTunnel::LaunchChecklist::DeployShield:
				case LaunchTunnel::LaunchChecklist::Launch: mfd->Write("waits for launch clearance", 14, 2);break;
				case LaunchTunnel::LaunchChecklist::Beacons: mfd->Write("is starting engines", 14, 2);break;
				case LaunchTunnel::LaunchChecklist::Speeding: mfd->Write("is accelerating", 14, 2);break;
				case LaunchTunnel::LaunchChecklist::TakeOff: mfd->Write("is airborne and clears area", 14, 2);break;
				}
				break;
			}
			break;
		default:
			//implicitly set prepare checklist subject			
			hangar->GetChecklist(0)->SetSubject(vessel->GetHandle());
			mfd->Write("Gate occupied - wait for clearance");
			break;
		}
	}

	char *LabelRenderer (int bt)
	{
		int state=GetChecklistStates();
		if (state==0 && (prepare==LaunchTunnel::PrepareChecklist::CloseEntry || prepare==LaunchTunnel::PrepareChecklist::Occupied))
		{
			//Call default renderer for passenger and fuel list
			switch (bt)
			{
				case 6: return "HOM";
				case 7: return "BCK";
				case 8: return "ABT";
				case 9: return "GO";
				case 11: return mode==MODEPERSONS?"FUE":"PAX"; //Overrides default rendering of PRV button
				default: return AscensionTowerPage::LabelRenderer(bt);
			}
		}
		switch (bt)
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 10:
			case 11: return "";
			case 6: return "HOM";
			case 7: return "BCK";
			case 8: return (state==0 && prepare==LaunchTunnel::PrepareChecklist::AbortOpen) || (state==1 && launch==LaunchTunnel::LaunchChecklist::AbortOpen)?"":"ABT";
			case 9: return (state==0 && prepare==LaunchTunnel::PrepareChecklist::Ready)?"RVT":"";
			default: return NULL;
		}
	}

	int MenuRenderer (MFDBUTTONMENU *mnu)
	{	
		static MFDBUTTONMENU menu[8] = 
		{
			{"Main menu", NULL, 'H'},
			{"Go back", NULL, 'B'},
			{"Abort checklist", "procedure", 'A'},
			{"Proceed to", "launch", 'G'},
			{NULL, NULL, 0},
			{"Switch to fuel", "mode", 'M'},
			{"Switch to PAX", "mode", 'M'},
			{"Revert to", "boarding", 'R'},
		};
		
		int state=GetChecklistStates();
		int k=	(state==0 && prepare==LaunchTunnel::PrepareChecklist::AbortOpen) ||
				(state==1 && launch==LaunchTunnel::LaunchChecklist::AbortOpen)?2:3; // In abort pages, just copy the first 2 right hand buttons, otherwise 3
		if (state==0 && (	prepare==LaunchTunnel::PrepareChecklist::CloseEntry ||
							prepare==LaunchTunnel::PrepareChecklist::Occupied		)) k=5; // In fuel or pax page, copy all button except the switch labels
		for(int i=0;i<k;i++) mnu[6+i]=menu[i];
		if (state==0 && prepare==LaunchTunnel::PrepareChecklist::Ready) mnu[6+k++]=menu[7];
		if (k<5) return 6+k;

		//Call default renderer for fuel or pax page
		mnu[0].line1=mode==MODEPERSONS?"EVA/Transfer person":"Modify level of tank";
		AscensionTowerPage::MenuRenderer(mnu);
		mnu[11]=menu[mode==MODEPERSONS?5:6]; //Overwrite the last button with mode switcher
		return 12;
	}

	AscensionTowerPageInstance ButtonHandler(int bt)
	{
		int state=GetChecklistStates();
		if (state==0 && (prepare==LaunchTunnel::PrepareChecklist::CloseEntry || prepare==LaunchTunnel::PrepareChecklist::Occupied))
		{
			//Call default renderer for passenger and fuel list
			switch (bt)
			{
			case 8: return SetKey(OAPI_KEY_A);
			case 9: return SetKey(OAPI_KEY_G);
			case 11: return SetKey(OAPI_KEY_M);
			default: return AscensionTowerPage::ButtonHandler(bt);
			}
		}
		switch (bt)
		{
			case 6: return SetKey(OAPI_KEY_H);
			case 7: return SetKey(OAPI_KEY_B);
			case 8: return SetKey(OAPI_KEY_A);
			case 9: return (state==0 && (prepare==LaunchTunnel::PrepareChecklist::Ready))?SetKey(OAPI_KEY_R):Undefined;
			default: return Undefined;
		}
	}

	AscensionTowerPageInstance KeyHandler(DWORD key)
	{
		int state=GetChecklistStates();
		if (state==0 && (prepare==LaunchTunnel::PrepareChecklist::CloseEntry || prepare==LaunchTunnel::PrepareChecklist::Occupied))
		{
			//Call default renderer for passenger and fuel list
			switch(key)
			{	
			case OAPI_KEY_H:
				return MainMenu;
			case OAPI_KEY_B:
				return GroundMenu;
			case OAPI_KEY_A:
				hangar->GetChecklist(0)->SetEvent(LaunchTunnel::PrepareChecklist::Abort);
				return NoChange;
			case OAPI_KEY_G:
				hangar->GetChecklist(0)->SetEvent(LaunchTunnel::PrepareChecklist::Proceed);
				return NoChange;
			case OAPI_KEY_M:
				if (mode==MODEPERSONS) mode=MODEPROPELLANT;
				else mode=MODEPERSONS;
				return NoChange;
			default:
				return AscensionTowerPage::KeyHandler(key);
			}		
		}
		switch(key)
		{	
		case OAPI_KEY_H:
			return MainMenu;
		case OAPI_KEY_B:
			return GroundMenu;
		case OAPI_KEY_R:
			if (state==0 && prepare==LaunchTunnel::PrepareChecklist::Ready) hangar->GetChecklist(0)->SetEvent(LaunchTunnel::PrepareChecklist::Revert);
			else return Undefined;
			return NoChange;			
		case OAPI_KEY_A:
			switch (state)
			{
			case 0:
				if (prepare!=LaunchTunnel::PrepareChecklist::AbortOpen) hangar->GetChecklist(0)->SetEvent(LaunchTunnel::PrepareChecklist::Abort);
				else return Undefined;
				return NoChange;
			case 1:
				if (launch!=LaunchTunnel::LaunchChecklist::AbortOpen)
				{
					Checklist *list=hangar->GetChecklist(0);
					list->SetSubject(vessel->GetHandle()); //Set the subject of the prepare checklist in order to abort even if empty 
					list->SetEvent(LaunchTunnel::PrepareChecklist::Abort);
					hangar->GetChecklist(1)->SetEvent(LaunchTunnel::LaunchChecklist::Abort);
				}
				else return Undefined;
				return NoChange;
			}
			//Fall through here
		default: return Undefined;
		}	
		return NoChange;
	}

	char *GetTitle(){return GetNameSafeTitle("Winged Launch");}

	char *GetSubtitle()
	{
		switch (GetChecklistStates())
		{
		case 1: //already subject of launch checklist, check state
			switch (launch)
			{
			case LaunchTunnel::LaunchChecklist::AbortOpen: return "Status: Abort";
			case LaunchTunnel::LaunchChecklist::Empty: return "Status: Empty";
			case LaunchTunnel::LaunchChecklist::OpenExit: return "Status: Opening Exit";
			case LaunchTunnel::LaunchChecklist::Exit: return "Status: Exiting";
			case LaunchTunnel::LaunchChecklist::CloseExit: return "Status: Closing Exit";
			case LaunchTunnel::LaunchChecklist::DeployShield: return "Status: Deploying Shield";
			}
			return "Status: Waiting";
		case 0: //only subject of prepare checklist, check state
			switch (prepare)
			{
			case LaunchTunnel::PrepareChecklist::AbortOpen: return "Status: Abort";
			case LaunchTunnel::PrepareChecklist::Empty: return "Status: Empty";
			case LaunchTunnel::PrepareChecklist::OpenEntry: return "Status: Opening Entry";
			case LaunchTunnel::PrepareChecklist::Entry: return "Status: Entry";
			case LaunchTunnel::PrepareChecklist::CloseEntry: return mode==MODEPERSONS?"Status: Closing Entry - Boarding":"Status: Closing Entry - Fuelling";
			case LaunchTunnel::PrepareChecklist::Occupied: return mode==MODEPERSONS?"Status: Boarding":"Status: Fuelling";
			case LaunchTunnel::PrepareChecklist::Ready: return "Status: Ready";
			}
		default: return "Status: Waiting";
		}
	}

	int GetListSize(){return mode==MODEPERSONS?((Hangar *)dataRoot)->GetRoom(0)->GetPersons():vessel->GetPropellantCount();}

	AscensionTowerListPair GetListItem(int index)
	{
		static char text[57];
		AscensionTowerListPair item=
		{
			index,
			text
		};

		if (mode==MODEPERSONS)
		{
			Person person=ascension->GetPerson(index, ((Hangar *)dataRoot)->GetRoom(0));
			sprintf(text, "  %s %s", person.MiscId, person.Name);
		}
		else
		{
			PROPELLANT_HANDLE propellant=vessel->GetPropellantHandleByIndex(index);
			double mass=vessel->GetPropellantMass(propellant);
			double maxmass=vessel->GetPropellantMaxMass(propellant);
			int level=(int)(mass/maxmass*100.0);
			sprintf(text, "%.2d %.0lfkg/%.0lfkg (%d%%)", index+1, mass, maxmass, level);
		}
		
		return item;
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		AscensionTowerPage::Select(index);
		if (mode==MODEPERSONS)
		{
			switch (ascension->ChangePerson(selectedIndex, PERSON_EVA | PERSON_FILTER, ((Hangar *)dataRoot)->GetRoom(0)))
			{
			case ERROR_DOCKEDSHIP_DONOT_USE_UMMU:
			case ERROR_DOCKED_SHIP_HAVE_AIRLOCK_CLOSED:
			case ERROR_DOCKED_SHIP_IS_FULL:
				//TODO: denial sound
			case EVA_OK:
				return NoChange;
			default: return Undefined;
			}
		}
		
		setTankLevel.Index=selectedIndex;
		setTankLevel.Data=vessel;
		PROPELLANT_HANDLE propellant=vessel->GetPropellantHandleByIndex(selectedIndex);
		sprintf(buffer, "%lf", vessel->GetPropellantMass(propellant));
		char line[BUFFERLEN+1];
		sprintf(line, "Change tank %d level ( *|%%*|f|e|empty ):", selectedIndex+1);
		oapiOpenInputBox(line, SetTankLevel, buffer, 26, (void *)&setTankLevel);
		return NoChange;
	}

	AscensionTowerPageInstance EventHandler(BaseVessel::EventHandler::Arguments args)
	{
		if (args.SourceType!=BaseVessel::EventHandler::Checklist) return NoChange;
		Checklist *list=(Checklist *)args.Source;
		Hangar *hangar=list->GetHangar();
		if (hangar->GetType()!=HANGARTYPELFMC) return NoChange;
		//Check event and subject according to list
		OBJHANDLE handle=vessel->GetHandle();
		if (hangar->GetChecklist(0)==list)
		{
			if (args.Event!=LaunchTunnel::PrepareChecklist::Aborted) return NoChange;			
		}
		else
		{
			if (args.Event!=LaunchTunnel::LaunchChecklist::Aborted &&
				args.Event!=LaunchTunnel::LaunchChecklist::Left) return NoChange;
		}
		if (list->GetSubject()!=handle) return NoChange;
		return HangarForLaunchSelection;
	}

private:	
	AscensionTowerCallbackData setTankLevel;
	char buffer[BUFFERLEN+1];
	int mode;
	Hangar *hangar;
	LaunchTunnel::LaunchChecklist::State launch;
	LaunchTunnel::PrepareChecklist::State prepare;

	int GetChecklistStates()
	{
		hangar=(Hangar *)dataRoot;
		Checklist *list=hangar->GetChecklist(1);
		OBJHANDLE handle=vessel->GetHandle();
		if (list->GetSubject()==handle)
		{
			launch=(LaunchTunnel::LaunchChecklist::State) list->GetState();
			return 1;
		}
		list=hangar->GetChecklist(0);
		if (list->GetSubject()==handle)
		{
			prepare=(LaunchTunnel::PrepareChecklist::State) list->GetState();
			return 0;
		}
		return -1;
	}
};