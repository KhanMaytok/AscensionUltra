#pragma once
#include "AscensionTowerPage.h"
#include "../LFMCChecklistStates.h"
#pragma warning(disable : 4482)
#define BUFFERLEN 256

class LaunchTunnelWizardPage: public AscensionTowerPage
{

public:

	LaunchTunnelWizardPage(AscensionTowerData *data):AscensionTowerPage(data){}

protected:

	void MFDRenderer()
	{
		mfd->SetWriteStyle(0, 2);
		Hangar *hangar=(Hangar *)dataRoot;
		Checklist *list=hangar->GetChecklist(1);
		if (list->GetSubject()==vessel->GetHandle())
		{
			//already subject of launch checklist, check state
			LaunchTunnel::LaunchChecklist::State state=(LaunchTunnel::LaunchChecklist::State) list->GetState();
			switch (state)
			{
			case LaunchTunnel::LaunchChecklist::Empty:
			case LaunchTunnel::LaunchChecklist::OpenExit: mfd->Write("Launchway free - wait for clearance");break;

			case LaunchTunnel::LaunchChecklist::Exit: mfd->Write("Clear to enter - taxi to hold");break;

			case LaunchTunnel::LaunchChecklist::CloseExit:
			case LaunchTunnel::LaunchChecklist::DeployShield: mfd->Write("Contact ATC for clearance");break;

			default: mfd->Write("Contact ATC for launch");break;
			}
		}
		else
		{
			list=hangar->GetChecklist(0);
			//implicitly set prepare checklist subject
			OBJHANDLE subject=vessel->GetHandle();
			list->SetSubject(subject);
			if (list->GetSubject()==subject)
			{
				//really subject of prepare checklist, check state
				LaunchTunnel::PrepareChecklist::State state=(LaunchTunnel::PrepareChecklist::State) list->GetState();
				switch (state)
				{
				case LaunchTunnel::PrepareChecklist::Empty:
				case LaunchTunnel::PrepareChecklist::OpenEntry: mfd->Write("Gate free - wait for clearance");break;

				case LaunchTunnel::PrepareChecklist::Entry: mfd->Write("Clear to enter - taxi to gate");break;

				case LaunchTunnel::PrepareChecklist::CloseEntry:
				case LaunchTunnel::PrepareChecklist::Occupied:
					mfd->Write("Person and Propellant refill");
					//TODO: display person and propellant list
					break;
				case LaunchTunnel::PrepareChecklist::Ready:
					mfd->Write("Launchway occupied - wait for clearance");
					//TODO: display "Launchway occupied - wait for clearance", launch subject name and launch status
				}
			}
			else mfd->Write("Gate occupied - wait for clearance");
		}

		//mfd->Write("Name", AT_BUTTON[0], 1, WRITEMFD_HALFLINES);		
	}

	/*char *LabelRenderer (int bt)
	{
		switch (bt)
		{
			case 0: return "NAM";
			case 1: return "FNC";
			case 2: return "AGE";
			case 3: return "PUL";
			case 4: return "WGT";
			case 5: return "LOC";
			case 6: return "HOM";
			case 7: return "BCK";
			case 8: return "RES";
			case 9: return "";
			case 10: return (int)dataRoot>0?"EVA":"";
			case 11: return (int)dataRoot>0?"DEL":"";
			default: return NULL;
		}
	}

	int MenuRenderer (MFDBUTTONMENU *mnu)
	{	
		static MFDBUTTONMENU menu[12] = {
			{"Change person", "name", 'N'},
			{menu[0].line1, "function", 'F'},
			{menu[0].line1, "age", 'A'},
			{menu[0].line1, "puls", 'P'},
			{menu[0].line1, "weigth", 'W'},
			{menu[0].line1, "location", 'L'},
			{"Main menu", NULL, 'H'},
			{"Go back", NULL, 'B'},
			{"Reset to default", "values", 'R'},
			{NULL, NULL, 0},
			{"EVA person", NULL, 'E'},
			{"Remove person", "from roster", 'D'}};

		for(int i=0;i<12;i++) mnu[i]=menu[i];
		if ((int)dataRoot==0)
		{
			mnu[10]=menu[9];
			mnu[11]=menu[9];
		}
		return 12;
	}

	AscensionTowerPageInstance ButtonHandler(int bt)
	{
		switch(bt)
		{
		case 0: return SetKey(OAPI_KEY_N);
		case 1: return SetKey(OAPI_KEY_F);
		case 2: return SetKey(OAPI_KEY_A);
		case 3: return SetKey(OAPI_KEY_P);
		case 4: return SetKey(OAPI_KEY_W);		
		case 5: return SetKey(OAPI_KEY_L);
		case 6: return SetKey(OAPI_KEY_H);
		case 7: return SetKey(OAPI_KEY_B);
		case 8: return SetKey(OAPI_KEY_R);
		case 10: return SetKey(OAPI_KEY_E);
		case 11: return SetKey(OAPI_KEY_D);
		default: return Undefined;
		}
	}

	AscensionTowerPageInstance KeyHandler(DWORD key)
	{
		int index = (int)dataRoot;
		changePerson.Index=index;
		changePerson.Data=data;
		switch(key)
		{
		case OAPI_KEY_N:
			changePerson.Flags=PERSON_NAME;
			oapiOpenInputBox("Change person name:", ChangePersonData,
				strncpy(buffer, ascension->GetPerson(index).Name, BUFFERLEN),
				26, (void *)&changePerson);
			return NoChange;
		case OAPI_KEY_F:
			changePerson.Flags=PERSON_MISCID;
			oapiOpenInputBox("Change function (Crew,Capt,Sek,Vip,Sci,Doc,Tech):", ChangePersonData,
				strncpy(buffer, ascension->GetPerson(index).MiscId, BUFFERLEN),
				26, (void *)&changePerson);
			return NoChange;
		case OAPI_KEY_A:
			changePerson.Flags=PERSON_AGE;
			sprintf(buffer, "%d", ascension->GetPerson(index).Age);
			oapiOpenInputBox("Change person age:", ChangePersonData, buffer, 26, (void *)&changePerson);
			return NoChange;
		case OAPI_KEY_P:
			changePerson.Flags=PERSON_PULS;
			sprintf(buffer, "%d", ascension->GetPerson(index).Puls);
			oapiOpenInputBox("Change person puls:", ChangePersonData, buffer, 26, (void *)&changePerson);
			return NoChange;
		case OAPI_KEY_W:
			changePerson.Flags=PERSON_WEIGHT;
			sprintf(buffer, "%d", ascension->GetPerson(index).Weight);
			oapiOpenInputBox("Change person weight:", ChangePersonData, buffer, 26, (void *)&changePerson);
			return NoChange;
		case OAPI_KEY_L:
			data->GetPage(HangarForPersonSelection)->SetDataRoot(dataRoot);
			return HangarForPersonSelection;
		case OAPI_KEY_E:
			if (index==0) return NoChange;
			switch (ascension->ChangePerson(index, PERSON_EVA))
			{
			case ERROR_DOCKEDSHIP_DONOT_USE_UMMU:
			case ERROR_DOCKED_SHIP_HAVE_AIRLOCK_CLOSED:
			case ERROR_DOCKED_SHIP_IS_FULL:
				//TODO: denial sound				
				return NoChange;
			case EVA_OK:
				return Roster;
			default: return Undefined;
			}
		case OAPI_KEY_D:
			if (index==0) return NoChange;
			ascension->ChangePerson(index, PERSON_DELETE);
			return Roster;
		case OAPI_KEY_B: return Roster;
		case OAPI_KEY_H: return MainMenu;
		case OAPI_KEY_R:
			//Reset();
			return NoChange;
		default: return Undefined;
		}
	} */

	char *GetTitle(){return GetNameSafeTitle("Ground");}

	char *GetSubtitle(){return "Winged Launch Procedure";}

private:	
	char buffer[BUFFERLEN+1];
};