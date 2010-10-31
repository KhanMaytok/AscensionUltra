#include "AscensionTowerData.h"

AscensionTowerData::AscensionTowerData(MFD* mfd)
{
	ascensionHandle=NULL;
	ascensionName=NULL;
	ascension=NULL;
	for(int i=0;i<STATES;i++)
	{
		page[i]=0;
		selection[i]=0;
		selectedIndex[i]=0;
		object[i]=NULL;
	}
	state=AscensionTowerState::MainMenu;
	changePerson.Data=this;
	this->mfd=mfd;
}

AscensionTowerData::~AscensionTowerData(void)
{
	delete [] ascensionName;
	for (std::vector<AscensionTowerListPair>::iterator i=scanList.begin(); i!=scanList.end(); i++) delete [] i->Name;
	scanList.clear();
}

// Resolves object reference by means of:
// 1. using the index, check pointer
// 2. get all vessels of class AscensionUltra, check for unique name
// 3. return NULL
AscensionUltra *AscensionTowerData::GetAscension()
{	
	if (oapiIsVessel(ascensionHandle)) return ascension;
	Scan();
	int detected=-1;
	if (ascensionName!=NULL) for (std::vector<AscensionTowerListPair>::iterator i=scanList.begin(); i!=scanList.end(); i++)
	{
		if (strcmp(i->Name, ascensionName)==0)
		{
			if (detected>=0)
			{
				SetState(AscensionTowerState::BaseSelect);
				return ascension=NULL; //Name is not unique
			}
			detected=i->Index;			
		}
	}
	if (detected<0)
	{
		if (scanList.size()!=1)
		{
			SetState(AscensionTowerState::BaseSelect);
			return ascension=NULL;
		}
		detected=scanList.begin()->Index;
	}
	SetAscension(detected);
	SetState(ascension==NULL?AscensionTowerState::BaseSelect:AscensionTowerState::MainMenu);
	return ascension;
}

int AscensionTowerData::GetPage(){return page[state];}
void AscensionTowerData::SetPage(int page){this->page[state]=page;}

void AscensionTowerData::SetAscension(int index)
{
	ascension=NULL;
	if (index<0 || index>=(int)oapiGetVesselCount()) return Scan();
	ascensionHandle=oapiGetVesselByIndex(index);
	VESSEL *vessel=oapiGetVesselInterface(ascensionHandle);	
	if (strcmp(vessel->GetClassName(), "AscensionUltra")!=0) return Scan();
	ascension=(AscensionUltra *)vessel;
	char *name=ascension->GetName();
	double version=ascension->GetVersion();
	if (version!=1.0)
	{
		ascension=NULL;
		sprintf(oapiDebugString(), "%s has version %f (need 1.0). Please upgrade MFD and/or vessel.", name, version);
		return Scan();
	}
	delete [] ascensionName;
	strcpy(ascensionName=new char[strlen(name)+1], name);	
}

void AscensionTowerData::Scan()
{
	for (std::vector<AscensionTowerListPair>::iterator i=scanList.begin(); i!=scanList.end(); i++) delete [] i->Name;
	scanList.clear();
	int k=oapiGetVesselCount();
	char line[20]; //enough for int conversion base 10
	char *target;
	for (int i=0;i<k;i++)
	{
		VESSEL *vessel=oapiGetVesselInterface(oapiGetVesselByIndex(i));	
		if (strcmp(vessel->GetClassName(), "AscensionUltra")==0)
		{
			char *source=vessel->GetName();
			int l=strlen(itoa(i, line, 10));
			int sum=strlen(source)+l+3;
			if (sum<37) sprintf(target=new char[sum+1], "[%s] %s", line, source);
			else
			{
				sprintf(target=new char[37], "[%s] ", line);
				strncat(target, source, 31-l);
				strcat(target, "..");
			}
			AscensionTowerListPair pair={i,target};
			scanList.push_back(pair);
		}
	}	
}

int AscensionTowerData::GetListSize()
{
	switch(state)
	{
	case AscensionTowerState::BaseSelect: return scanList.size();
	case AscensionTowerState::MainMenu: return 4;
	case AscensionTowerState::GroundMenu: return 4;
	case AscensionTowerState::ATCMenu: return 3;
	case AscensionTowerState::HangarForDoorSelection:	
		return ascension->GetHangars(HangarType::TurnAround)+ascension->GetHangars(HangarType::LightStorage)+ascension->GetHangars(HangarType::LaunchTunnel);
	case AscensionTowerState::HangarForRoomSelection:
	case AscensionTowerState::HangarForCraneSelection: return ascension->GetHangars(HangarType::TurnAround);
	case AscensionTowerState::DoorSelection: return ((Hangar *)object[state])->GetDoors();		
	case AscensionTowerState::RoomSelection: return ((Hangar *)object[state])->GetRooms();
	case AscensionTowerState::DoorControl: return 3;
	case AscensionTowerState::TaxiRouteStartSelection: return ascension->GetTaxiways()->GetPoints();
	case AscensionTowerState::TaxiRouteEndSelection: return ascension->GetTaxiways()->GetPoints(false, (char *)object[state]);
	case AscensionTowerState::LandingRunwaySelection: return ascension->GetRunways()->GetPoints();
	case AscensionTowerState::Rooster: return ascension->GetPersons();				
	}
	return 0;
}

AscensionTowerListPair AscensionTowerData::GetListItem(int index)
{
	static AscensionTowerListPair mainMenu[4]={{0," Ground Operations"},{1," Air Traffic Control"},{2," Control Rooms"}, {3, " Rooster"}};
	static AscensionTowerListPair groundMenu[4]={{0," Request Roll-in/Roll-out"},{1," Request Taxi"},{2," Request Cargo Control"},{3," Request Launch"}};
	static AscensionTowerListPair atcMenu[3]={{0," Request Bearing"},{1," Request Clearance to Land"},{2," Request Launch Clearance"}};
	static AscensionTowerListPair doorMenu[3]={{0," Open"},{1," Close"},{2," Stop"}};	
	static char text[57];

	AscensionTowerListPair nullItem={0,""};	
	AscensionTowerListPair item;
	
	switch(state)
	{
	case AscensionTowerState::BaseSelect: return scanList[index];
	case AscensionTowerState::MainMenu: return mainMenu[index];
	case AscensionTowerState::GroundMenu: return groundMenu[index];
	case AscensionTowerState::ATCMenu: return atcMenu[index];
	case AscensionTowerState::DoorControl: return doorMenu[index];
	case AscensionTowerState::HangarForDoorSelection:
		item.Index=index;
		item.Name=ascension->GetHangar(index<17?(index<5?HangarType::TurnAround:HangarType::LightStorage):HangarType::LaunchTunnel, index<17?(index<5?index:index-5):index-17)->GetName();
		return item;
	case AscensionTowerState::DoorSelection:
		item.Index=index;
		item.Name=((Hangar *)object[state])->GetDoor(index)->GetName();
		return item;
	case AscensionTowerState::HangarForRoomSelection:
		item.Index=index;
		{
			Hangar *h=ascension->GetHangar(HangarType::TurnAround, index);
			item.Name=h->GetName();
			sprintf(text, "%c %s", ascension->GetControlRoom()->GetHangar()==h?'*':' ', item.Name);
		}
		item.Name=text;
		return item;
	case AscensionTowerState::RoomSelection:
		item.Index=index;
		{
			Room *r=((Hangar *)object[state])->GetRoom(index);
			item.Name=r->GetName();
			sprintf(text, "%c %s", ascension->GetControlRoom()==r?'*':' ', item.Name);
		}
		item.Name=text;
		return item;
	case AscensionTowerState::HangarForCraneSelection:
		item.Index=index;
		item.Name=ascension->GetHangar(HangarType::TurnAround, index)->GetName();
		return item;
	case AscensionTowerState::TaxiRouteStartSelection:
		item.Index=index;
		{
			Routes *t=ascension->GetTaxiways();
			item.Name=t->GetPoint(index);
			sprintf(text, "%c %s", t->AnyStrobing(item.Name)?'*':' ', item.Name);
		}
		item.Name=text;
		return item;
	case AscensionTowerState::TaxiRouteEndSelection:
		item.Index=index;
		{
			Routes *t=ascension->GetTaxiways();
			item.Name=t->GetPoint(index, false, (char *)object[state]);
			sprintf(text, "%c %s", t->Strobing((char *)object[state], item.Name)?'*':' ', item.Name);
		}
		item.Name=text;
		return item;
	case AscensionTowerState::LandingRunwaySelection:
		item.Index=index;
		{
			Routes *t=ascension->GetRunways();
			item.Name=t->GetPoint(index);
			sprintf(text, "%c %s", t->AnyStrobing(item.Name)?'*':' ', item.Name);
		}
		item.Name=text;
		return item;
	case AscensionTowerState::Rooster:
		item.Index=index;
		{
			Person person=ascension->GetPerson(index);
			if (index>0) sprintf(text, "  %s %s", person.MiscId, person.Name);
			else sprintf(text, "  Add new person...");
		}
		item.Name=text;
		return item;	
	}
	return nullItem;
}

int AscensionTowerData::GetSelection(){return selection[state];}

AscensionTowerState AscensionTowerData::GetState(){return state;}

void AscensionTowerData::SetState(AscensionTowerState state)
{
	switch(state)
	{
	case AscensionTowerState::BaseSelect:
		Scan();
		break;	
	}
	this->state=state;
}

void AscensionTowerData::Select()
{
	selectedIndex[state]=GetListItem(page[state]*6+selection[state]).Index;
	int index;
	char *start, *end;
	Routes *t;
	switch(state)
	{
	case AscensionTowerState::BaseSelect:
		SetAscension(selectedIndex[state]);
		SetState(AscensionTowerState::MainMenu);
		break;	
	case AscensionTowerState::MainMenu:
		switch(selection[state])
		{
		case 0: SetState(AscensionTowerState::GroundMenu); break;
		case 1: SetState(AscensionTowerState::ATCMenu); break;
		case 2: SetState(AscensionTowerState::HangarForRoomSelection); break;
		case 3: SetState(AscensionTowerState::Rooster); break;
		}		
		break;
	case AscensionTowerState::GroundMenu:
		switch(selection[state])
		{
		case 0: SetState(AscensionTowerState::HangarForDoorSelection); break;
		case 1: SetState(AscensionTowerState::TaxiRouteStartSelection); break;
		case 2: SetState(AscensionTowerState::HangarForCraneSelection); break;
		case 3: SetState(AscensionTowerState::PassengerTerminal); break;
		}		
		break;
	case AscensionTowerState::ATCMenu:
		switch(selection[state])
		{
		case 0: SetState(AscensionTowerState::Bearing); break;
		case 1: SetState(AscensionTowerState::LandingRunwaySelection); break;
		case 2: SetState(AscensionTowerState::Launch); break;
		}		
		break;
	case AscensionTowerState::HangarForDoorSelection:
		index=selectedIndex[state];
		object[AscensionTowerState::DoorSelection]=ascension->GetHangar(index<17?(index<5?HangarType::TurnAround:HangarType::LightStorage):HangarType::LaunchTunnel, index<17?(index<5?index:index-5):index-17);
		SetState(AscensionTowerState::DoorSelection);		
		break;
	case AscensionTowerState::HangarForRoomSelection:
		index=selectedIndex[state];
		object[AscensionTowerState::RoomSelection]=ascension->GetHangar(HangarType::TurnAround, index);
		SetState(AscensionTowerState::RoomSelection);		
		break;
	case AscensionTowerState::RoomSelection:
		object[AscensionTowerState::DoorControl]=((Hangar *)object[state])->GetDoor(selectedIndex[state]);
		ascension->SwitchView(((Hangar *)object[state])->GetRoom(selectedIndex[state]));
		break;
	case AscensionTowerState::DoorSelection:
		object[AscensionTowerState::DoorControl]=((Hangar *)object[state])->GetDoor(selectedIndex[state]);
		SetState(AscensionTowerState::DoorControl);
		break;
	case AscensionTowerState::HangarForCraneSelection:
		object[state]=ascension->GetHangar(HangarType::TurnAround, selectedIndex[state]);
		object[AscensionTowerState::CraneControl]=((TurnAroundHangar *)object[state])->GetCrane();
		SetState(AscensionTowerState::CraneControl);
		break;
	case AscensionTowerState::DoorControl:
		switch(selection[state])
		{
		case 0: ((Door *)object[state])->Open(); break;
		case 1: ((Door *)object[state])->Close(); break;
		case 2: ((Door *)object[state])->Stop(); break;
		}		
		break;
	case AscensionTowerState::TaxiRouteStartSelection:
		object[AscensionTowerState::TaxiRouteEndSelection]=ascension->GetTaxiways()->GetPoint(selectedIndex[state]);
		SetState(AscensionTowerState::TaxiRouteEndSelection);
		break;
	case AscensionTowerState::TaxiRouteEndSelection:
		t=ascension->GetTaxiways();
		start=(char *)object[AscensionTowerState::TaxiRouteEndSelection];
		end=t->GetPoint(selectedIndex[state], false, start);
		if (t->Strobing(start,end)) t->Strobe(start, end, false);
		else
		{
			t->Strobe(false);
			t->Strobe(start, end, true);
		}
		break;
	case AscensionTowerState::LandingRunwaySelection:
		t=ascension->GetRunways();
		start=t->GetPoint(selectedIndex[state]);
		for(int i=t->GetPoints(false, start)-1;i>=0;i--) if ((end=t->GetPoint(i, false, start))[0]=='L') break; //Search for endpoint starting with "L" => finding pointer for Lead-in
		t->Strobe(start, end, !t->Strobing(start,end));		
		break;
	case AscensionTowerState::Rooster:
		selectedIndex[AscensionTowerState::PersonControl]=selectedIndex[state];
		SetState(AscensionTowerState::PersonControl);
		break;
	}
}

// Return button labels
char *AscensionTowerData::GetButtonLabel (int bt)
{
	switch(state)
	{
	case AscensionTowerState::CraneControl:
		switch (bt)
		{
			case 0: return "X+";
			case 1: return "X-";
			case 2: return "Y+";
			case 3: return "Y-";
			case 4: return "Z+";
			case 5: return "Z-";
			case 6: return "STP";
			case 7: return "SPD";
			case 8: return "CWL";
			case 9: return "DIR";
			case 10: return "LST";
			case 11: return "BCK";
		}
		return NULL;
	case AscensionTowerState::PersonControl:
		switch (bt)
		{
			case 0: return "NAM";
			case 1: return "FNC";
			case 2: return "AGE";
			case 3: return "PUL";
			case 4: return "WGT";
			case 5: return "LOC";
			case 6: return "";
			case 7: return selectedIndex[AscensionTowerState::Rooster]>0?"EVA":"";
			case 8: return "";
			case 9: return selectedIndex[AscensionTowerState::Rooster]>0?"DEL":"";
			case 10: return "";
			case 11: return "BCK";
		}
		return NULL;
	default:
		int size=GetListSize();
		switch (bt)
		{
			case 6: return size>0?"SEL":"";
			case 7: return size>1?"UP":"";
			case 8: return size>1?"DWN":"";
			case 9: return size>6?"NXT":"";
			case 10: return size>6?"PRV":"";
			case 11: return (state==AscensionTowerState::BaseSelect || state==AscensionTowerState::MainMenu)?"BAS":"BCK";
			default: return size>page[state]*6+bt?" > ":"";
		}
		return NULL;
	}
}


// Return button menus
int AscensionTowerData::GetButtonMenu (MFDBUTTONMENU *mnu)
{	
	static char select[20];
	static char marked[20];
	static char *target;
	static MFDBUTTONMENU craneControlMenu[12] = {
		{"Move X axis", "pos. by step", 'A'},
		{"Move X axis", "neg. by step", 'D'},
		{"Move Y axis", "pos. by step", 'W'},
		{"Move Y axis", "neg. by step", 'S'},
		{"Move Z axis", "up by step", 'Q'},
		{"Move Z axis", "down by step", 'E'},
		{"Toggle step", "size", 'R'},
		{"Set speed", NULL, 'F'},
		{"Set crawl", NULL, 'V'},
		{"Direct key", "command", 'C'},
		{"Goto list", "page", 'X'},
		{"Go back", NULL, 'B'}};
	static MFDBUTTONMENU personMenu[12] = {
		{"Change person", "name", 'N'},
		{personMenu[0].line1, "function", 'F'},
		{personMenu[0].line1, "age", 'A'},
		{personMenu[0].line1, "puls", 'P'},
		{personMenu[0].line1, "weigth", 'W'},
		{personMenu[0].line1, "location", 'L'},
		{NULL, NULL, 0},
		{"EVA person", NULL, 'E'},
		{NULL, NULL, 0},
		{"Remove person", "from rooster", 'D'},
		{NULL, NULL, 0},
		{"Go back", NULL, 'B'}};

	switch(state)
	{	
	case AscensionTowerState::BaseSelect: target="base"; break;		
	case AscensionTowerState::GroundMenu: 
	case AscensionTowerState::ATCMenu:
	case AscensionTowerState::MainMenu:  target="request"; break;
	case AscensionTowerState::Rooster: target="person"; break;
	case AscensionTowerState::HangarForCraneSelection:
	case AscensionTowerState::HangarForRoomSelection:
	case AscensionTowerState::HangarForDoorSelection: target="hangar"; break;
	case AscensionTowerState::DoorSelection: target="door"; break;
	case AscensionTowerState::TaxiRouteStartSelection: target="route start"; break;
	case AscensionTowerState::TaxiRouteEndSelection: target="route end"; break;
	case AscensionTowerState::LandingRunwaySelection: target="runway"; break;
	case AscensionTowerState::RoomSelection: target="room"; break;
	case AscensionTowerState::DoorControl: target="command"; break;
	case AscensionTowerState::CraneControl:
		for(int i=0;i<12;i++) mnu[i]=craneControlMenu[i];
		return 12;
	case AscensionTowerState::PersonControl:
		for(int i=0;i<12;i++) mnu[i]=personMenu[i];
		if (selectedIndex[AscensionTowerState::Rooster]==0)
		{
			mnu[7]=personMenu[6];
			mnu[9]=personMenu[6];
		}
		return 12;
	}
	sprintf(select, "Select %s", target);
	sprintf(marked, "marked %s", target);

	int size=GetListSize();
	int k=min(size-page[state]*6, 6);

	for(int i=0;i<k;i++)
	{
		mnu[i].line1=select;
		mnu[i].line2="next to the button";
		mnu[i].selchar=0x31+i;
	}
	for(int i=k;i<6;i++)
	{
		mnu[i].line1=NULL;
		mnu[i].line2=NULL;
		mnu[i].selchar=0;
	}
	
	k=6;
	if (size>0)
	{
		mnu[6].line1="Select currently";
		mnu[6].line2=marked;
		mnu[6].selchar='S';
		k=7;
	}
	if (size>1)
	{
		mnu[7].line1="Move marker";
		mnu[7].line2="up";
		mnu[7].selchar='U';
		mnu[8].line1="Move marker";
		mnu[8].line2="down";
		mnu[8].selchar='D';
		k=9;
	}
	for(int i=k;i<9;i++)
	{
		mnu[i].line1=NULL;
		mnu[i].line2=NULL;
		mnu[i].selchar=0;
	}

	if (size>6)
	{
		mnu[9].line1="Switch to";
		mnu[9].line2="next page";
		mnu[9].selchar='N';
		mnu[10].line1="Switch to";
		mnu[10].line2="previous page";
		mnu[10].selchar='P';
	}
	else for(int i=9;i<11;i++)
	{
		mnu[i].line1=NULL;
		mnu[i].line2=NULL;
		mnu[i].selchar=0;
	}

	mnu[11].line1=(state==AscensionTowerState::BaseSelect || state==AscensionTowerState::MainMenu)?"Scan for bases":"Go back";
	mnu[11].line2=NULL;
	mnu[11].selchar='B';
	
	return 12;
}

// Handling button presses
bool AscensionTowerData::SetButton(int bt)
{
	switch(state)
	{
	case AscensionTowerState::CraneControl:
		switch(bt)
		{
		case 0: return SetKey(OAPI_KEY_A);
		case 1: return SetKey(OAPI_KEY_D);
		case 2: return SetKey(OAPI_KEY_W);
		case 3: return SetKey(OAPI_KEY_S);
		case 4: return SetKey(OAPI_KEY_Q);
		case 5: return SetKey(OAPI_KEY_E);
		case 6: return SetKey(OAPI_KEY_R);
		case 7: return SetKey(OAPI_KEY_F);
		case 8: return SetKey(OAPI_KEY_V);
		case 9: return SetKey(OAPI_KEY_C);
		case 10: return SetKey(OAPI_KEY_X);
		case 11: return SetKey(OAPI_KEY_B);	
		}
		break;
	case AscensionTowerState::PersonControl:
		switch(bt)
		{
		case 0: return SetKey(OAPI_KEY_N);
		case 1: return SetKey(OAPI_KEY_F);
		case 2: return SetKey(OAPI_KEY_A);
		case 3: return SetKey(OAPI_KEY_P);
		case 4: return SetKey(OAPI_KEY_W);		
		case 5: return SetKey(OAPI_KEY_L);
		case 7: return SetKey(OAPI_KEY_E);
		case 9: return SetKey(OAPI_KEY_D);
		case 11: return SetKey(OAPI_KEY_B);
		}
		break;
	default:
		switch(bt)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5: return SetKey(OAPI_KEY_1+bt);
		case 6: return SetKey(OAPI_KEY_S);
		case 7: return SetKey(OAPI_KEY_U);
		case 8: return SetKey(OAPI_KEY_D);
		case 9: return SetKey(OAPI_KEY_N);
		case 10: return SetKey(OAPI_KEY_P);
		case 11: return SetKey(OAPI_KEY_B);	
		}
		break;
	}
	return false;
}

bool ChangePersonData(void *id, char *str, void *usrdata)
{
	AscensionTowerChangePerson *cp=(AscensionTowerChangePerson *)usrdata;
	AscensionTowerData *data=cp->Data;
	data->GetAscension()->ChangePerson(data->GetSelectedIndex(), cp->Flags, str);
	data->GetMfd()->InvalidateDisplay();
	return true;
}

// Handling shortcut keys
bool AscensionTowerData::SetKey(DWORD key)
{	
	bool result=true;
	int size=GetListSize();
	int pages=(size+5)/6;
	switch(state)
	{
	case AscensionTowerState::CraneControl:
		switch(key)
		{
		case OAPI_KEY_A:
			break;
		case OAPI_KEY_D:
			break;
		case OAPI_KEY_W:
			break;
		case OAPI_KEY_S:
			break;
		case OAPI_KEY_Q:
			break;
		case OAPI_KEY_E:
			break;
		case OAPI_KEY_R:
			break;
		case OAPI_KEY_F:
			break;
		case OAPI_KEY_V:
			break;
		case OAPI_KEY_C:
			((Crane*)GetObject())->StartManual();
			break;
		case OAPI_KEY_X:			
			break;
		case OAPI_KEY_B:
			Back();
			break;
		default:
			result=false;
			break;
		}
		return result;

	case AscensionTowerState::PersonControl:
		switch(key)
		{
		case OAPI_KEY_N:
			changePerson.Flags=PERSON_NAME;
			oapiOpenInputBox("Change person name:", ChangePersonData, strncpy(buffer, ascension->GetPerson(selectedIndex[AscensionTowerState::Rooster]).Name, BUFFERLEN), 26, (void *)&changePerson);
			break;
		case OAPI_KEY_F:
			changePerson.Flags=PERSON_MISCID;
			oapiOpenInputBox("Change function (Crew,Capt,Sek,Vip,Sci,Doc,Tech):", ChangePersonData, strncpy(buffer, ascension->GetPerson(selectedIndex[AscensionTowerState::Rooster]).MiscId, BUFFERLEN), 26, (void *)&changePerson);
			break;
		case OAPI_KEY_A:
			changePerson.Flags=PERSON_AGE;
			sprintf(buffer, "%d", ascension->GetPerson(selectedIndex[AscensionTowerState::Rooster]).Age);
			oapiOpenInputBox("Change person age:", ChangePersonData, buffer, 26, (void *)&changePerson);
			break;
		case OAPI_KEY_P:
			changePerson.Flags=PERSON_PULS;
			sprintf(buffer, "%d", ascension->GetPerson(selectedIndex[AscensionTowerState::Rooster]).Puls);
			oapiOpenInputBox("Change person puls:", ChangePersonData, buffer, 26, (void *)&changePerson);
			break;
		case OAPI_KEY_W:
			changePerson.Flags=PERSON_WEIGHT;
			sprintf(buffer, "%d", ascension->GetPerson(selectedIndex[AscensionTowerState::Rooster]).Weight);
			oapiOpenInputBox("Change person weight:", ChangePersonData, buffer, 26, (void *)&changePerson);
			break;
		case OAPI_KEY_L:
			break;
		case OAPI_KEY_E:
			if (selectedIndex[AscensionTowerState::Rooster]==0) return false;
			ascension->ChangePerson(selectedIndex[AscensionTowerState::Rooster], PERSON_EVA);
			SetState(AscensionTowerState::Rooster);
			break;
		case OAPI_KEY_D:
			if (selectedIndex[AscensionTowerState::Rooster]==0) return false;
			ascension->ChangePerson(selectedIndex[AscensionTowerState::Rooster], PERSON_DELETE);
			SetState(AscensionTowerState::Rooster);
			break;
		case OAPI_KEY_B:
			Back();
			break;
		default:
			result=false;
			break;
		}
		return result;
		
	default:
		switch(key)
		{	
		case OAPI_KEY_N://Next page
			if (size>6)
			{
				if (page[state]<pages-1) page[state]++;
				else page[state]=0;
				selection[state]=0;
			}
			else result=false;		
			break;
		case OAPI_KEY_P://Previous page
			if (size>6)
			{
				if (page[state]>0) page[state]--;
				else page[state]=pages-1;
				selection[state]=min(size-page[state]*6, 6)-1;
			}
			else result=false;		
			break;
		case OAPI_KEY_U://Selection up
			if (size>1)
			{
				if (selection[state]>0) selection[state]--;
				else
				{
					SetKey(OAPI_KEY_P);
					selection[state]=min(size-page[state]*6, 6)-1;
				}
			}
			else result=false;
			break;
		case OAPI_KEY_D://Selection down
			if (size>1)
			{
				if (selection[state]<min(size-page[state]*6, 6)-1) selection[state]++;
				else
				{
					SetKey(OAPI_KEY_N);
					selection[state]=0;
				}
			}
			else result=false;
			break;
		case OAPI_KEY_S://Select
			if (size>0) Select();
			else result=false;
			break;
		case OAPI_KEY_B://Go back/Scan for bases
			Back();
			break;
		default:
			if (key>=OAPI_KEY_1 && key<=OAPI_KEY_6)
			{
				int bt=key-OAPI_KEY_1;
				if (bt<min(size-page[state]*6, 6))
				{
					selection[state]=bt;
					Select();
				}
				else result=false;
			}
			else result=false;
		}
		return result;
	}
	return false;
}

char *AscensionTowerData::GetNameSafeTitle(char *title, char *trailer)
{
	char *name=ascension->GetName();
	int i=56-strlen(trailer);
	bool longer=false;
	if (strlen(name)>i)
	{
		i-=2;
		longer=true;
	}
	strncpy(title, name, i);
	title[i]=0;
	if (longer) strcat(title, "..");
	strcat(title, trailer);
	return title;
}

char *AscensionTowerData::GetTitle()
{
	static char *tower=" Tower";
	static char *ground=" Ground";
	static char *atc=" ATC";
	static char *rooster=" Rooster";
	static char title[57];
	switch(state)
	{
	case AscensionTowerState::BaseSelect:
		sprintf(title, "Ascension%s", tower);
		return title;
	case AscensionTowerState::GroundMenu:
	case AscensionTowerState::HangarForDoorSelection:
	case AscensionTowerState::DoorSelection:
	case AscensionTowerState::TaxiRouteStartSelection:
	case AscensionTowerState::TaxiRouteEndSelection:
	case AscensionTowerState::HangarForCraneSelection:
	case AscensionTowerState::PassengerTerminal:
	case AscensionTowerState::Fueling:
	case AscensionTowerState::LaunchPrepare:
	case AscensionTowerState::DoorControl:
	case AscensionTowerState::CraneControl:
		return GetNameSafeTitle(title, ground);	
	case AscensionTowerState::ATCMenu:
	case AscensionTowerState::Bearing:
	case AscensionTowerState::LandingRunwaySelection:
	case AscensionTowerState::Launch:
		return GetNameSafeTitle(title, atc);	
		break;
	case AscensionTowerState::Rooster:
	case AscensionTowerState::PersonControl:
		return GetNameSafeTitle(title, rooster);
		break;
	case AscensionTowerState::MainMenu:
	case AscensionTowerState::HangarForRoomSelection:
	case AscensionTowerState::RoomSelection:
	default:		
		return GetNameSafeTitle(title, tower);	
	}	
}

char *AscensionTowerData::GetSubtitle()
{
	static char subtitle[57];

	switch(state)
	{
	case AscensionTowerState::BaseSelect: return scanList.size()>0?"Select base":"";
	case AscensionTowerState::MainMenu: return "Select request";
	case AscensionTowerState::GroundMenu: return "Select ground request";
	case AscensionTowerState::ATCMenu: return "Select ATC request";	
	case AscensionTowerState::Rooster: return "Select Person";
	case AscensionTowerState::PersonControl: return selectedIndex[AscensionTowerState::Rooster]>0?"Person Information":"Add new person";
	case AscensionTowerState::HangarForDoorSelection: return "Select Hangar for Roll-in/Roll-out";	
	case AscensionTowerState::DoorSelection: return "Select Door for Roll-in/Roll-out";	
	case AscensionTowerState::TaxiRouteStartSelection: return "Select Taxi Route Start";
	case AscensionTowerState::HangarForCraneSelection: return "Select Hangar for Cargo Crane";	
	case AscensionTowerState::PassengerTerminal: return "Passenger Terminal";	
	case AscensionTowerState::Fueling: return "Fueling";	
	case AscensionTowerState::LaunchPrepare: return "Launch Tunnel";	
	case AscensionTowerState::Bearing: return "Bearing";	
	case AscensionTowerState::LandingRunwaySelection: return "Select Runway for Landing";	
	case AscensionTowerState::Launch: return "Request Launch Clearance";	
	case AscensionTowerState::HangarForRoomSelection: return "Select Hangar for Control Room";
	case AscensionTowerState::RoomSelection: return "Select Control Room";
	case AscensionTowerState::TaxiRouteEndSelection:
		sprintf(subtitle, "Taxi from %s to",
			(char *)object[state]);
		return subtitle;
	case AscensionTowerState::DoorControl:
		sprintf(subtitle, "%s -> %s",
			((Hangar *)object[AscensionTowerState::DoorSelection])->GetName(),
			((Door *)object[state])->GetName());
		return subtitle;
	case AscensionTowerState::CraneControl:
		sprintf(subtitle, "%s -> Crane",
			((Hangar *)object[AscensionTowerState::HangarForCraneSelection])->GetName());
		return subtitle;
	}
	return "";
}

void AscensionTowerData::Back()
{
	switch(state)
	{
	case AscensionTowerState::MainMenu:
	case AscensionTowerState::BaseSelect: SetState(AscensionTowerState::BaseSelect);break;	
	case AscensionTowerState::GroundMenu: SetState(AscensionTowerState::MainMenu);break;
	case AscensionTowerState::ATCMenu: SetState(AscensionTowerState::MainMenu);break;
	case AscensionTowerState::Rooster: SetState(AscensionTowerState::MainMenu);break;
	case AscensionTowerState::PersonControl: SetState(AscensionTowerState::Rooster);break;
	case AscensionTowerState::HangarForDoorSelection: SetState(AscensionTowerState::GroundMenu);break;
	case AscensionTowerState::DoorSelection: SetState(AscensionTowerState::HangarForDoorSelection);break;
	case AscensionTowerState::DoorControl: SetState(AscensionTowerState::DoorSelection);break;
	case AscensionTowerState::TaxiRouteStartSelection: SetState(AscensionTowerState::GroundMenu);break;
	case AscensionTowerState::TaxiRouteEndSelection: SetState(AscensionTowerState::TaxiRouteStartSelection);break;
	case AscensionTowerState::HangarForCraneSelection: SetState(AscensionTowerState::GroundMenu);break;
	case AscensionTowerState::CraneControl: SetState(AscensionTowerState::HangarForCraneSelection);break;
	case AscensionTowerState::PassengerTerminal: SetState(AscensionTowerState::GroundMenu);break;
	case AscensionTowerState::Fueling: SetState(AscensionTowerState::PassengerTerminal);break;
	case AscensionTowerState::LaunchPrepare: SetState(AscensionTowerState::Fueling);break;
	case AscensionTowerState::Bearing: SetState(AscensionTowerState::ATCMenu);break;
	case AscensionTowerState::LandingRunwaySelection: SetState(AscensionTowerState::ATCMenu);break;
	case AscensionTowerState::Launch: SetState(AscensionTowerState::ATCMenu);break;
	case AscensionTowerState::HangarForRoomSelection: SetState(AscensionTowerState::MainMenu);break;
	case AscensionTowerState::RoomSelection: SetState(AscensionTowerState::HangarForRoomSelection);break;
	}
}

void *AscensionTowerData::GetObject(){return object[state];}

int AscensionTowerData::GetSelectedIndex(){return selectedIndex[state];}

MFD *AscensionTowerData::GetMfd(){return mfd;}