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

	LaunchTunnelWizardPage(AscensionTowerData *data):AscensionTowerPage(data){ticker=false;}

protected:

	void MFDRenderer()
	{
		ticker=!ticker;
		mfd->SetWriteStyle(0, 2);
		GetChecklistStates();
		switch(listType)
		{
		case LaunchTunnel::Request:
			switch ((LaunchTunnel::RequestChecklist::State)state)
			{
			case LaunchTunnel::RequestChecklist::LFHold:
				mfd->Write("Clearance granted,", 10);
				mfd->Write("Proceed to LF stop line", 11);
				break;
			case LaunchTunnel::RequestChecklist::Wait:
				mfd->Write("Pre-Flight Hold occupied", 10);
				mfd->Write(GetTicker("awaiting clearance"), 11);
				break;
			}
			break;
		case LaunchTunnel::Preflight:
			switch((LaunchTunnel::PreflightChecklist::State)state)
			{
			case LaunchTunnel::PreflightChecklist::OpenEntry:
				mfd->Write("Request granted,", 10);
				mfd->Write(GetTicker("awaiting clearance"), 11);
				break;
			case LaunchTunnel::PreflightChecklist::Entry:
				mfd->Write("Clearance granted,", 10);
				mfd->Write("Proceed to Pre-Flight Hold", 11);
				break;
			case LaunchTunnel::PreflightChecklist::PFHold:
				mfd->Write("Carry out pre-flight checks", 10);				
				break;
			case LaunchTunnel::PreflightChecklist::Wait:
				mfd->Write("Passenger Hold occupied,", 10);
				mfd->Write(GetTicker("awaiting clearance"), 11);
				break;
			case LaunchTunnel::PreflightChecklist::AbortOpen:
				mfd->Write("Pre-Flight aborted,", 10);
				mfd->Write(GetTicker("Proceed to Escape Hold"), 11);
				break;
			}
			break;
		case LaunchTunnel::Boarding:
			switch((LaunchTunnel::BoardingChecklist::State)state)
			{
			case LaunchTunnel::BoardingChecklist::Taxi:
				mfd->Write("Clearance granted,", 10);
				mfd->Write("Proceed to Passenger Hold", 11);
				break;
			case LaunchTunnel::BoardingChecklist::PAXHold:
				AscensionTowerPage::MFDRenderer(); //Simply do default list rendering
				break;
			case LaunchTunnel::BoardingChecklist::Wait:
				mfd->Write("Fueling Hold occupied,", 10);
				mfd->Write(GetTicker("awaiting clearance"), 11);
				break;
			case LaunchTunnel::BoardingChecklist::AbortWait:
				mfd->Write("Boarding aborted,", 10);
				mfd->Write(GetTicker("Proceed to Escape Hold"), 11);
				break;
			}
			break;
		case LaunchTunnel::Fueling:
			switch((LaunchTunnel::FuelingChecklist::State)state)
			{
			case LaunchTunnel::FuelingChecklist::Taxi:
				mfd->Write("Clearance granted,", 10);
				mfd->Write("Proceed to Fueling Hold", 11);
				break;
			case LaunchTunnel::FuelingChecklist::FuelHold:
				AscensionTowerPage::MFDRenderer(); //Simply do default list rendering
				break;
			case LaunchTunnel::FuelingChecklist::Wait:
				mfd->Write("Launch Hold occupied,", 10);
				mfd->Write(GetTicker("awaiting clearance"), 11);
				break;
			case LaunchTunnel::FuelingChecklist::AbortWait:
				mfd->Write("Fueling aborted,", 10);
				mfd->Write(GetTicker("Proceed to Escape Hold"), 11);
				break;
			}
			break;
		case LaunchTunnel::Launch:
			switch((LaunchTunnel::LaunchChecklist::State)state)
			{
			case LaunchTunnel::LaunchChecklist::OpenExit:
				mfd->Write("Request granted,", 10);
				mfd->Write(GetTicker("awaiting clearance"), 11);
				break;
			case LaunchTunnel::LaunchChecklist::Exit:
				mfd->Write("Clearance granted,", 10);
				mfd->Write("Proceed to Launch Hold", 11);
				break;
			case LaunchTunnel::LaunchChecklist::Blast:
				mfd->Write("Closing blast door,", 10);
				mfd->Write(GetTicker("awaiting clearance"), 11);
				break;
			case LaunchTunnel::LaunchChecklist::LaunchHold:
			case LaunchTunnel::LaunchChecklist::Beacons:
			case LaunchTunnel::LaunchChecklist::Speeding:
			case LaunchTunnel::LaunchChecklist::TakeOff:
				mfd->Write("Launchway clear.", 10);
				mfd->Write("Contact Air Traffic Control", 11);
				mfd->Write("for launch clearance.", 12);
				break;
			case LaunchTunnel::LaunchChecklist::AbortOpen:
				mfd->Write("Launch aborted,", 10);
				mfd->Write(GetTicker("Proceed to Escape Hold"), 11);
				break;
			}
			break;
		default:
			hangar->GetChecklist(0)->SetSubject(vessel->GetHandle());
			mfd->Write(GetTicker("Awaiting clearance"), 10);
			break;
		}
	}

	char *LabelRenderer (int bt)
	{
		GetChecklistStates();
		
		if (isSelect)
		{
			//Call default renderer for passenger and fuel list
			switch (bt)
			{
				case 6: return "HOM";
				case 7: return "BCK";
				case 8: return "ABT";
				case 9: return "GO";
				case 11: return listType==LaunchTunnel::Boarding?"ROS":"STP";
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
			case 8: return noAbort?"":"ABT";
			case 9: return hasGo?"GO":"";
			default: return NULL;
		}
	}

	int MenuRenderer (MFDBUTTONMENU *mnu)
	{	
		static MFDBUTTONMENU menu[7] = 
		{
			{"Main menu", NULL, 'H'},
			{"Go back", NULL, 'B'},
			{"Abort checklist", "procedure", 'A'},
			{"Proceed to", "next hold", 'G'},
			{NULL, NULL, 0},
			{"Switch to person", "roster page", 'S'},
			{"Stop all fueling", NULL, 'S'},
		};
		
		GetChecklistStates();
		int k=noAbort?2:		// Without abort button, just copy the first 2 right hand buttons, otherwise
			(hasGo?4:			// with proceed button, copy 4 right hand buttons, otherwise	
			(isSelect?5:3));	// in select pages, copy 5 entries, with the last one clearing the NXT button for the default renderer
		
		for(int i=0;i<k;i++) mnu[6+i]=menu[i];
		if (k<5) return 6+k;

		//Call default renderer for fuel or pax page
		mnu[0].line1=listType==LaunchTunnel::Boarding?"EVA/Transfer person":"Change propellant level";
		AscensionTowerPage::MenuRenderer(mnu);
		mnu[11]=menu[listType==LaunchTunnel::Boarding?5:6]; //Overwrite the last button with mode switcher
		return 12;
	}

	AscensionTowerPageInstance ButtonHandler(int bt)
	{
		GetChecklistStates();
		if (isSelect)
		{
			//Call default renderer for passenger and fuel list
			switch (bt)
			{
			case 8: return SetKey(OAPI_KEY_A);
			case 9: return SetKey(OAPI_KEY_G);
			case 11: return SetKey(OAPI_KEY_S);
			default: return AscensionTowerPage::ButtonHandler(bt);
			}
		}
		switch (bt)
		{
			case 6: return SetKey(OAPI_KEY_H);
			case 7: return SetKey(OAPI_KEY_B);
			case 8: return noAbort?Undefined:SetKey(OAPI_KEY_A);
			case 9: return hasGo?SetKey(OAPI_KEY_G):Undefined;
			default: return Undefined;
		}
	}

	AscensionTowerPageInstance KeyHandler(DWORD key)
	{
		GetChecklistStates();
		if (isSelect)
		{
			//Call default renderer for passenger and fuel list
			switch(key)
			{	
			case OAPI_KEY_H:
				return MainMenu;
			case OAPI_KEY_B:
				return GroundMenu;
			case OAPI_KEY_A:
				//TODO: implement abort display change here
				list->SetEvent(LaunchTunnel::BoardingChecklist::Abort); //Takes advantage of boarding and fueling list having the same event definition
				return NoChange;
			case OAPI_KEY_G:
				list->SetEvent(LaunchTunnel::BoardingChecklist::Proceed); //Takes advantage of boarding and fueling list having the same event definition
				return NoChange;
			case OAPI_KEY_S:
				if (listType==LaunchTunnel::Boarding) return Roster;
				//TODO: stop fillings here by setting target levels to current levels
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
		case OAPI_KEY_G:
			if (!hasGo) return Undefined;
			list->SetEvent(LaunchTunnel::PreflightChecklist::Proceed); //Could only be the preflight list currently
			return NoChange;
		case OAPI_KEY_A:
			if (noAbort) return Undefined;
			{
				int event=0;
				switch (listType)
				{
				case LaunchTunnel::Request:   event=LaunchTunnel::RequestChecklist::Abort; break;
				case LaunchTunnel::Preflight: event=LaunchTunnel::PreflightChecklist::Abort; break;
				case LaunchTunnel::Boarding:  event=LaunchTunnel::BoardingChecklist::Abort; break;
				case LaunchTunnel::Fueling:   event=LaunchTunnel::FuelingChecklist::Abort; break;
				case LaunchTunnel::Launch:    event=LaunchTunnel::LaunchChecklist::Abort; break;
				}
				list->SetEvent(event);
			}
			return NoChange;
		default: return Undefined;
		}
	}

	char *GetTitle(){return GetNameSafeTitle("Winged Launch");}

	char *GetSubtitle()
	{
		switch(listType)
		{
		case LaunchTunnel::Request:
			switch ((LaunchTunnel::RequestChecklist::State)state)
			{
			case LaunchTunnel::RequestChecklist::LFHold: return "Status: Taxi to LF";
			case LaunchTunnel::RequestChecklist::Wait: return "Status: Wait";
			}
			break;
		case LaunchTunnel::Preflight:
			switch((LaunchTunnel::PreflightChecklist::State)state)
			{
			case LaunchTunnel::PreflightChecklist::OpenEntry: return "Status: Opening Entry";
			case LaunchTunnel::PreflightChecklist::Entry: return "Status: Taxi to Pre-Flight";
			case LaunchTunnel::PreflightChecklist::PFHold: return "Status: Pre-Flight check";
			case LaunchTunnel::PreflightChecklist::Wait: return "Status: Wait";
			case LaunchTunnel::PreflightChecklist::AbortOpen: return "Status: ABORT";
			}
			break;
		case LaunchTunnel::Boarding:
			switch((LaunchTunnel::BoardingChecklist::State)state)
			{
			case LaunchTunnel::BoardingChecklist::Taxi: return "Status: Taxi to PAX";
			case LaunchTunnel::BoardingChecklist::PAXHold: return "Status: Boarding";
			case LaunchTunnel::BoardingChecklist::Wait: return "Status: Wait";
			case LaunchTunnel::BoardingChecklist::AbortWait: return "Status: ABORT";
			}
			break;
		case LaunchTunnel::Fueling:
			switch((LaunchTunnel::FuelingChecklist::State)state)
			{
			case LaunchTunnel::FuelingChecklist::Taxi: return "Status: Taxi to Fueling";
			case LaunchTunnel::FuelingChecklist::FuelHold: return "Status: Fueling";
			case LaunchTunnel::FuelingChecklist::Wait: return "Status: Wait";
			case LaunchTunnel::FuelingChecklist::AbortWait: return "Status: ABORT";
			}
			break;
		case LaunchTunnel::Launch:
			switch((LaunchTunnel::LaunchChecklist::State)state)
			{
			case LaunchTunnel::LaunchChecklist::OpenExit: return "Status: Opening Exit";
			case LaunchTunnel::LaunchChecklist::Exit: return "Status: Taxi to Launch";
			case LaunchTunnel::LaunchChecklist::Blast: return "Status: Blast Shielding";
			case LaunchTunnel::LaunchChecklist::LaunchHold:
			case LaunchTunnel::LaunchChecklist::Beacons:
			case LaunchTunnel::LaunchChecklist::Speeding:
			case LaunchTunnel::LaunchChecklist::TakeOff: return "Status: Ready to Launch";
			case LaunchTunnel::LaunchChecklist::AbortOpen: return "Status: ABORT";
			}
			break;
		default: return "Status: Request";
		}
	}

	int GetListSize(){return listType==LaunchTunnel::Boarding?((Hangar *)dataRoot)->GetRoom(0)->GetPersons():vessel->GetPropellantCount();}

	AscensionTowerListPair GetListItem(int index)
	{
		static char text[57];
		AscensionTowerListPair item=
		{
			index,
			text
		};

		if (listType==LaunchTunnel::Boarding)
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
		if (listType==LaunchTunnel::Boarding)
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
		if (args.SourceType!=BaseVessel::EventHandler::Checklist) return Undefined;
		Checklist *list=(Checklist *)args.Source;
		Hangar *hangar=list->GetHangar();
		if (hangar->GetType()!=HANGARTYPELFMC) return Undefined;
		//Check event and subject according to list
		OBJHANDLE handle=vessel->GetHandle();
		if (list->GetSubject()!=handle) return Undefined;
		if (args.Event==LaunchTunnel::Step) return NoChange; //Refresh display
		if (args.Event!=LaunchTunnel::Aborted && args.Event!=LaunchTunnel::Left) return Undefined;
		return HangarForLaunchSelection;
	}

private:	
	AscensionTowerCallbackData setTankLevel;
	char buffer[BUFFERLEN+1];
	bool ticker;
	Hangar *hangar;
	LaunchTunnel::ListType listType;
	Checklist *list;
	int state;
	bool noAbort, hasGo, isSelect;
	
	void GetChecklistStates()
	{
		list=NULL;
		listType=LaunchTunnel::None;
		state=-1;
		hangar=(Hangar *)dataRoot;
		int lists=ascension->GetChecklists(hangar, vessel);
		if (lists<1) return;
		list=ascension->GetChecklist(hangar, lists-1, vessel);

		listType=(LaunchTunnel::ListType)list->GetType();
		state=list->GetState();
		
		noAbort=
			(listType==LaunchTunnel::Request) ||
			(listType==LaunchTunnel::Preflight && (LaunchTunnel::PreflightChecklist::State)state==LaunchTunnel::PreflightChecklist::AbortOpen) ||
			(listType==LaunchTunnel::Boarding && (LaunchTunnel::BoardingChecklist::State)state==LaunchTunnel::BoardingChecklist::AbortWait) ||
			(listType==LaunchTunnel::Fueling && (LaunchTunnel::FuelingChecklist::State)state==LaunchTunnel::FuelingChecklist::AbortWait) ||
			(listType==LaunchTunnel::Launch && ((LaunchTunnel::LaunchChecklist::State)state==LaunchTunnel::LaunchChecklist::AbortOpen ||
												(LaunchTunnel::LaunchChecklist::State)state==LaunchTunnel::LaunchChecklist::TakeOff));
		
		hasGo=listType==LaunchTunnel::Preflight && (LaunchTunnel::PreflightChecklist::State)state==LaunchTunnel::PreflightChecklist::PFHold;

		isSelect=
			(listType==LaunchTunnel::Boarding && (LaunchTunnel::BoardingChecklist::State)state==LaunchTunnel::BoardingChecklist::PAXHold) ||
			(listType==LaunchTunnel::Fueling && (LaunchTunnel::FuelingChecklist::State)state==LaunchTunnel::FuelingChecklist::FuelHold);
	}

	char *GetTicker(const char* text)
	{
		static char line[BUFFERLEN]="";
		strcpy(line, text);
		strcat(line, ticker?". . .":" . . .");
		return line;
	}
};