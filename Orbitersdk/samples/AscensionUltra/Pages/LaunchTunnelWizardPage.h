// Copyright (C) 2010-2014 Friedrich Kastner-Masilko
//
// This file is part of the Ascension Ultra Project
//
// Ascension Ultra is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of
// the License, or (at your option) any later version.
//
// Ascension Ultra is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Ascension Ultra. If not, see
// <http://www.gnu.org/licenses/>.

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

	LaunchTunnelWizardPage(AscensionTowerData *data):AscensionTowerPage(data)
	{
		ticker=false;
		abortConfirmation=false;
	}

protected:

	void SelectionRenderer()
	{
		char line[40];
		int size=GetListSize();
		int pages=(size+3)/4;
		if (page>=pages)
		{
			page=max(pages-1,0);
			mfd->InvalidateButtons();
		}
			
		mfd->SetWriteStyle(0);
		for(int i=0; i+page*4<size && i<4; i++)
			mfd->Write(GetListItem(i+page*4).Name, AT_BUTTON[i+1], 1, WRITEMFD_HALFLINES | (i==selection?WRITEMFD_HIGHLIGHTED:0));
		if (pages>0)
		{
			sprintf(line, "p.%d/%d", page+1, pages);
			mfd->Write(line, 27, -1, WRITEMFD_RIGHTALIGNED);
		}
		else
		{
			mfd->Write("No passengers in terminal,", 12);
			mfd->Write("select roster for check-in", 14);
		}
	}

#define MPROCEED mfd->SetWriteStyle(0); \
				 mfd->Write("PROCEED TO NEXT HOLD", 4);

#define MBRAKES mfd->SetWriteStyle(0, 3); \
				mfd->Write("HOLD HERE", 4); \
				mfd->Write("WHEEL BRAKES ON", 6);

	void MFDRenderer()
	{
		ticker=!ticker;
		if (abortConfirmation)
		{
			mfd->SetWriteStyle(0, 2);
			mfd->Write("DO YOU REALLY WISH TO", 11);
			mfd->Write("ABORT THE LAUNCH", 13);
			mfd->Write("PROCEDURE ?", 15);
			return;
		}
		GetChecklistStates();
		int i=27;
		if (!noAbort)
		{
			mfd->SetWriteStyle(0, 3);
			mfd->Write("press ABT to abort launch", i--);
		}
		if (hasGo || isSelect)
		{
			mfd->SetWriteStyle(0, 0);
			mfd->Write("press GO when ready to proceed", --i);
		}
		mfd->SetWriteStyle(0, 2);
		mfd->Write(GetWizardTitle(), 0);
		mfd->Write(GetWizardSubtitle(), 2);
		switch(listType)
		{
		case LaunchTunnel::Request:
			switch ((LaunchTunnel::RequestChecklist::State)state)
			{
			case LaunchTunnel::RequestChecklist::LFHold:
				mfd->Write("Clearance granted,", 12);
				mfd->Write("Proceed to LF stop line", 14);
				MPROCEED;
				break;
			case LaunchTunnel::RequestChecklist::Wait:
				mfd->Write("Pre-Flight Hold occupied", 12);
				mfd->Write(GetTicker("awaiting clearance"), 14);
				MBRAKES;
				break;
			}
			break;
		case LaunchTunnel::Preflight:
			switch((LaunchTunnel::PreflightChecklist::State)state)
			{
			case LaunchTunnel::PreflightChecklist::OpenEntry:
				mfd->Write("Request granted,", 12);
				mfd->Write(GetTicker("awaiting clearance"), 14);
				MBRAKES;
				break;
			case LaunchTunnel::PreflightChecklist::Entry:
				mfd->Write("Clearance granted,", 12);
				mfd->Write("Proceed to Pre-Flight Hold", 14);
				MPROCEED;
				break;
			case LaunchTunnel::PreflightChecklist::PFHold:
				mfd->Write("Carry out pre-flight checks", 13);
				MBRAKES;
				break;
			case LaunchTunnel::PreflightChecklist::Wait:
				mfd->Write("Passenger Hold occupied,", 12);
				mfd->Write(GetTicker("awaiting clearance"), 14);
				MBRAKES;
				break;
			case LaunchTunnel::PreflightChecklist::AbortOpen:
				mfd->Write("Pre-Flight aborted,", 12);
				mfd->Write(GetTicker("Proceed to Escape Hold"), 14);
				MPROCEED;
				break;
			}
			break;
		case LaunchTunnel::Boarding:
			switch((LaunchTunnel::BoardingChecklist::State)state)
			{
			case LaunchTunnel::BoardingChecklist::Taxi:
				mfd->Write("Clearance granted,", 12);
				mfd->Write("Proceed to Passenger Hold", 14);
				MPROCEED;
				break;
			case LaunchTunnel::BoardingChecklist::PAXHold:
				SelectionRenderer();
				MBRAKES;
				break;
			case LaunchTunnel::BoardingChecklist::Wait:
				mfd->Write("Fueling Hold occupied,", 12);
				mfd->Write(GetTicker("awaiting clearance"), 14);
				MBRAKES;
				break;
			case LaunchTunnel::BoardingChecklist::AbortWait:
				mfd->Write("Boarding aborted,", 12);
				mfd->Write(GetTicker("Proceed to Escape Hold"), 14);
				MPROCEED;
				break;
			}
			break;
		case LaunchTunnel::Fueling:
			switch((LaunchTunnel::FuelingChecklist::State)state)
			{
			case LaunchTunnel::FuelingChecklist::Taxi:
				mfd->Write("Clearance granted,", 12);
				mfd->Write("Proceed to Fueling Hold", 14);
				MPROCEED;
				break;
			case LaunchTunnel::FuelingChecklist::FuelHold:
				SelectionRenderer();
				MBRAKES;
				break;
			case LaunchTunnel::FuelingChecklist::Wait:
				mfd->Write("Launch Hold occupied,", 12);
				mfd->Write(GetTicker("awaiting clearance"), 14);
				MBRAKES;
				break;
			case LaunchTunnel::FuelingChecklist::AbortWait:
				mfd->Write("Fueling aborted,", 12);
				mfd->Write(GetTicker("Proceed to Escape Hold"), 14);
				MPROCEED;
				break;
			}
			break;
		case LaunchTunnel::Launch:
			switch((LaunchTunnel::LaunchChecklist::State)state)
			{
			case LaunchTunnel::LaunchChecklist::OpenExit:
				mfd->Write("Request granted,", 12);
				mfd->Write(GetTicker("awaiting clearance"), 14);
				MBRAKES;
				break;
			case LaunchTunnel::LaunchChecklist::Exit:
				mfd->Write("Clearance granted,", 12);
				mfd->Write("Proceed to Launch Hold", 14);
				MPROCEED;
				break;
			case LaunchTunnel::LaunchChecklist::Blast:
				mfd->Write("Closing blast door,", 12);
				mfd->Write(GetTicker("awaiting clearance"), 14);
				MBRAKES;
				break;
			case LaunchTunnel::LaunchChecklist::LaunchHold:
			case LaunchTunnel::LaunchChecklist::Beacons:
			case LaunchTunnel::LaunchChecklist::Speeding:
			case LaunchTunnel::LaunchChecklist::TakeOff:
				mfd->Write("Launchway clear.", 11);
				mfd->Write("Contact Air Traffic Control", 13);
				mfd->Write("for launch clearance.", 15);
				MBRAKES;
				break;
			case LaunchTunnel::LaunchChecklist::AbortOpen:
				mfd->Write("Launch aborted,", 12);
				mfd->Write(GetTicker("Proceed to Escape Hold"), 14);
				MPROCEED;
				break;
			}
			break;
		default:
			hangar->GetChecklist(0)->SetSubject(vessel->GetHandle());
			mfd->Write(GetTicker("Awaiting clearance"), 13);
			MBRAKES;
			break;
		}
	}

	char *LabelRenderer (int bt)
	{
		if (abortConfirmation) switch (bt)
		{
			case 0:
			case 1:			
			case 3:
			case 4:
			case 5:
			case 6:
			case 7: return "";
			case 2: return "YES";
			case 8: return "NO";
			default: return NULL;
		}		

		GetChecklistStates();
		
		if (isSelect) 
		{
			int size=GetListSize();
	
			switch (bt) //Call default renderer for passenger and fuel list
			{
				case 0:
				case 5: return NULL;
				case 1:
				case 2:
				case 3:
				case 4: return size>page*4+bt-1?" > ":NULL;
				case 6: return "HOM";
				case 7: return "BCK";
				case 8: return "ABT";
				case 9: return "GO";
				case 10: return size>4?"NXT":NULL;
				case 11: return listType==LaunchTunnel::Boarding?"ROS":"STP";
				default: return NULL;
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
		static MFDBUTTONMENU menu[14] = 
		{
			{NULL, "next to the button", '1'},
			{NULL, menu[0].line2, '2'},
			{NULL, menu[0].line2, '3'},
			{NULL, menu[0].line2, '4'},
			{"Main menu", NULL, 'H'},
			{"Go back", NULL, 'B'},
			{"Abort checklist", "procedure", 'A'},
			{"Proceed to", "next hold", 'G'},
			{NULL, NULL, 0},
			{"Switch to", "next page", 'N'},
			{"Switch to person", "roster page", 'S'},
			{"Stop all fueling", NULL, 'S'},
			{"Confirm abort", NULL, 'Y'},
			{"Cancel abort", NULL, 'N'},
		};

		if (abortConfirmation)
		{
			mnu[2]=menu[12];
			mnu[8]=menu[13];
			return 9;
		}
		
		GetChecklistStates();
		int k=noAbort?2:		// Without abort button, just copy the first 2 right hand buttons, otherwise
			(hasGo?4:			// with proceed button, copy 4 right hand buttons, otherwise	
			(isSelect?5:3));	// in select pages, copy 5 entries, with the last one clearing the NXT button for the default renderer
		
		for(int i=0;i<k;i++) mnu[6+i]=menu[i+4];
		if (k<5) return 6+k;

		//Renderer for fuel or pax page
		const char *label=listType==LaunchTunnel::Boarding?"EVA/Transfer person":"Change propellant level";
		for(int i=1;i<5;i++)
		{
			mnu[i]=menu[i-1];
			mnu[i].line1=label;
		}
		int size=GetListSize();
		for(int i=min(size-page*4, 4)+1;i<5;i++) mnu[i]=menu[8];
		if (size>4) mnu[10]=menu[9];		
		mnu[11]=menu[listType==LaunchTunnel::Boarding?10:11]; //Overwrite the last button with mode switcher
		return 12;
	}

	AscensionTowerPageInstance ButtonHandler(int bt)
	{
		if (abortConfirmation) return bt!=2?(bt!=8?Undefined:SetKey(OAPI_KEY_N)):SetKey(OAPI_KEY_Y);

		GetChecklistStates();
		if (isSelect) switch (bt) //Call default renderer for passenger and fuel list
		{	
		case 0:
		case 5: return Undefined;
		case 1:
		case 2:
		case 3:
		case 4: return SetKey(OAPI_KEY_1+bt-1);
		case 6: return SetKey(OAPI_KEY_H);
		case 7: return SetKey(OAPI_KEY_B);
		case 8: return SetKey(OAPI_KEY_A);
		case 9: return SetKey(OAPI_KEY_G);
		case 10: return SetKey(OAPI_KEY_N);
		case 11: return SetKey(OAPI_KEY_S);
		default: return Undefined;
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
		if (abortConfirmation)
		{
			if (key==OAPI_KEY_N)
			{
				abortConfirmation=false;
				return NoChange;
			}
			if (key==OAPI_KEY_Y)
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
				abortConfirmation=false;
				return NoChange;
			}
			return Undefined;
		}
		
		if (isSelect) //Renderer for passenger and fuel list
		{
			int size=GetListSize();
			int pages=(size+3)/4;
			switch(key)
			{
			case OAPI_KEY_H:
				return MainMenu;
			case OAPI_KEY_B:
				return GroundMenu;
			case OAPI_KEY_A:
				abortConfirmation=true;
				return NoChange;
			case OAPI_KEY_G:
				list->SetEvent(LaunchTunnel::BoardingChecklist::Proceed); //Takes advantage of boarding and fueling list having the same event definition
				return NoChange;
			case OAPI_KEY_S:
				if (listType==LaunchTunnel::Boarding) return Roster;
				//TODO: stop fillings here by setting target levels to current levels
				return NoChange;
			case OAPI_KEY_N://Next page
				if (size<5) return Undefined;
				if (page<pages-1) page++;
				else page=0;
				selection=0;
				return NoChange;
			default:
				if (key<OAPI_KEY_1 || key>OAPI_KEY_4) return Undefined;
				{
					int bt=key-OAPI_KEY_1;
					if (bt>=min(size-page*4, 4)) return Undefined;
					selection=bt;
					return Select();
				}
			}
		}
		
		switch(key)
		{	
		case OAPI_KEY_H:
			if (listType==LaunchTunnel::Request) list->SetEvent(LaunchTunnel::RequestChecklist::Abort);
			return MainMenu;
		case OAPI_KEY_B:
			if (listType!=LaunchTunnel::Request) return GroundMenu;
			list->SetEvent(LaunchTunnel::RequestChecklist::Abort);
			return HangarForLaunchSelection;
		case OAPI_KEY_G:
			if (!hasGo) return Undefined;
			list->SetEvent(LaunchTunnel::PreflightChecklist::Proceed); //Could only be the preflight list currently
			return NoChange;
		case OAPI_KEY_A:
			if (noAbort) return Undefined;
			abortConfirmation=true;
			return NoChange;
		default: return Undefined;
		}
	}

	char *GetTitle(){return "";}
	char *GetWizardTitle(){return GetNameSafeTitle("Winged Launch Control");}

	char *GetSubtitle(){return "";}
	char *GetWizardSubtitle()
	{
		if (abortConfirmation) return "Launch Phase: ABORT";
		switch(listType)
		{
		case LaunchTunnel::Request:
			switch ((LaunchTunnel::RequestChecklist::State)state)
			{
			case LaunchTunnel::RequestChecklist::LFHold: return "Launch Phase: Taxi to LF";
			case LaunchTunnel::RequestChecklist::Wait: return "Launch Phase: Wait";
			}
			break;
		case LaunchTunnel::Preflight:
			switch((LaunchTunnel::PreflightChecklist::State)state)
			{
			case LaunchTunnel::PreflightChecklist::OpenEntry: return "Launch Phase: Opening Entry";
			case LaunchTunnel::PreflightChecklist::Entry: return "Launch Phase: Taxi to Pre-Flight";
			case LaunchTunnel::PreflightChecklist::PFHold: return "Launch Phase: Pre-Flight check";
			case LaunchTunnel::PreflightChecklist::Wait: return "Launch Phase: Wait";
			case LaunchTunnel::PreflightChecklist::AbortOpen: return "Launch Phase: ABORT";
			}
			break;
		case LaunchTunnel::Boarding:
			switch((LaunchTunnel::BoardingChecklist::State)state)
			{
			case LaunchTunnel::BoardingChecklist::Taxi: return "Launch Phase: Taxi to PAX";
			case LaunchTunnel::BoardingChecklist::PAXHold: return "Launch Phase: Boarding";
			case LaunchTunnel::BoardingChecklist::Wait: return "Launch Phase: Wait";
			case LaunchTunnel::BoardingChecklist::AbortWait: return "Launch Phase: ABORT";
			}
			break;
		case LaunchTunnel::Fueling:
			switch((LaunchTunnel::FuelingChecklist::State)state)
			{
			case LaunchTunnel::FuelingChecklist::Taxi: return "Launch Phase: Taxi to Fueling";
			case LaunchTunnel::FuelingChecklist::FuelHold: return "Launch Phase: Fueling";
			case LaunchTunnel::FuelingChecklist::Wait: return "Launch Phase: Wait";
			case LaunchTunnel::FuelingChecklist::AbortWait: return "Launch Phase: ABORT";
			}
			break;
		case LaunchTunnel::Launch:
			switch((LaunchTunnel::LaunchChecklist::State)state)
			{
			case LaunchTunnel::LaunchChecklist::OpenExit: return "Launch Phase: Opening Exit";
			case LaunchTunnel::LaunchChecklist::Exit: return "Launch Phase: Taxi to Launch";
			case LaunchTunnel::LaunchChecklist::Blast: return "Launch Phase: Blast Shielding";
			case LaunchTunnel::LaunchChecklist::LaunchHold:
			case LaunchTunnel::LaunchChecklist::Beacons:
			case LaunchTunnel::LaunchChecklist::Speeding:
			case LaunchTunnel::LaunchChecklist::TakeOff: return "Launch Phase: Ready to Launch";
			case LaunchTunnel::LaunchChecklist::AbortOpen: return "Launch Phase: ABORT";
			}
			break;		
		}
		return "Launch Phase: Request";
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
	bool noAbort, hasGo, isSelect, abortConfirmation;
	
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