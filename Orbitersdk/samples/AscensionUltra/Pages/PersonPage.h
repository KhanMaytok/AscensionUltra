#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)
#define BUFFERLEN 256

// Global function for dialog callback
bool ChangePersonData(void *id, char *str, void *usrdata)
{
	AscensionTowerCallbackData *cp=(AscensionTowerCallbackData *)usrdata;
	AscensionTowerData *data=(AscensionTowerData *)cp->Data;
	int index=data->GetAscension()->ChangePerson(cp->Index, cp->Flags, str);
	data->GetPage(Roster)->SetIndex(index);
	data->GetMFD()->InvalidateDisplay();
	return true;
}

class PersonPage: public AscensionTowerPage
{

public:

	PersonPage(AscensionTowerData *data):AscensionTowerPage(data){}

protected:

	void MFDRenderer()
	{
		mfd->SetWriteStyle(1);
		mfd->Write("Name", AT_BUTTON[0], 1, WRITEMFD_HALFLINES);
		mfd->Write("Function", AT_BUTTON[1], 1, WRITEMFD_HALFLINES);
		mfd->Write("Age", AT_BUTTON[2], 1, WRITEMFD_HALFLINES);
		mfd->Write("Puls", AT_BUTTON[3], 1, WRITEMFD_HALFLINES);
		mfd->Write("Weight", AT_BUTTON[4], 1, WRITEMFD_HALFLINES);
		mfd->Write("Location", AT_BUTTON[5], 1, WRITEMFD_HALFLINES);

		mfd->SetWriteStyle(0, 2);
		char line[10];

		Person person=ascension->GetPerson((int)dataRoot);
		mfd->Write(person.Name, AT_BUTTON[0], 7, WRITEMFD_HALFLINES);
		mfd->Write(person.MiscId, AT_BUTTON[1], 7, WRITEMFD_HALFLINES);
		mfd->Write(_itoa(person.Age, line, 10), AT_BUTTON[2], 7, WRITEMFD_HALFLINES);
		mfd->Write(_itoa(person.Puls, line, 10), AT_BUTTON[3], 7, WRITEMFD_HALFLINES);
		mfd->Write(_itoa(person.Weight, line, 10), AT_BUTTON[4], 7, WRITEMFD_HALFLINES);
		mfd->Write(person.Location->GetHangar()->GetName(), AT_BUTTON[5]-1, 7, WRITEMFD_HALFLINES);
		mfd->Write(person.Location->GetName(), AT_BUTTON[5]+1, 11, WRITEMFD_HALFLINES);
	}

	char *LabelRenderer (int bt)
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
	}

	char *GetTitle(){return GetNameSafeTitle("Roster");}

	char *GetSubtitle(){return (int)dataRoot>0?"Person Information":"Add new person";}

private:
	AscensionTowerCallbackData changePerson;
	char buffer[BUFFERLEN+1];
};