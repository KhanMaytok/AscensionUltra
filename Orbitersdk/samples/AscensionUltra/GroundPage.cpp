#include "GroundPage.h"
#pragma warning(disable : 4482)

int GroundPage::GetListSize()
{
	return ascension->GetNearestHangar(HANGARTYPETA | HANGARTYPELFMC | HANGARTYPEPORT, vessel)==NULL?4:5;
	/*switch(state)
	{
	case AscensionTowerState::ATCMenu: return 3;
	case AscensionTowerState::HangarForDoorSelection:	
		return ascension->GetHangars(HANGARTYPETA | HANGARTYPELL | HANGARTYPELFMC | HANGARTYPELH | HANGARTYPEVLC);
	case AscensionTowerState::HangarForPersonSelection:
	case AscensionTowerState::HangarForRoomSelection: return ascension->GetHangars(HANGARTYPETA | HANGARTYPELFMC | HANGARTYPEPORT | HANGARTYPEVLC);
	case AscensionTowerState::HangarForCraneSelection: return ascension->GetHangars(HANGARTYPETA);
	case AscensionTowerState::DoorSelection: return ((Hangar *)object[state])->GetDoors();
	case AscensionTowerState::RoomForPersonSelection:
	case AscensionTowerState::RoomSelection: return ((Hangar *)object[state])->GetRooms();
	case AscensionTowerState::DoorControl: return 3;
	case AscensionTowerState::CraneList: return ((Crane *)object[state])->GetWaypoints();
	case AscensionTowerState::TaxiRouteStartSelection: return ascension->GetTaxiways()->GetPoints();
	case AscensionTowerState::TaxiRouteEndSelection: return ascension->GetTaxiways()->GetPoints(false, (char *)object[state]);
	case AscensionTowerState::LandingRunwaySelection: return ascension->GetRunways()->GetPoints();
	case AscensionTowerState::Roster: return ascension->GetPersons();
	case AscensionTowerState::PassengerTransfer:
		if (ascension->GetNearestHangar(HANGARTYPETA | HANGARTYPELFMC | HANGARTYPEPORT, vessel)!=(Hangar *)object[state])
		{
			SetState(AscensionTowerState::GroundMenu);
			return 0;
		}
		return ((Hangar *)object[state])->GetRooms();
	}
	return 0;*/
}

AscensionTowerListPair GroundPage::GetListItem(int index)
{
	static AscensionTowerListPair menu[5]={{0," Request Roll-in/Roll-out"},{1," Request Taxi"},{2," Request Cargo Control"},{3," Request Launch"},{4," Request Passenger Transfer"}};
	return menu[index];
	
	/*static AscensionTowerListPair atcMenu[3]={{0," Request Bearing"},{1," Request Clearance to Land"},{2," Request Launch Clearance"}};
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
		item.Name=ascension->GetHangar(HANGARTYPETA | HANGARTYPELL | HANGARTYPELFMC | HANGARTYPELH | HANGARTYPEVLC, index)->GetName();
		return item;
	case AscensionTowerState::DoorSelection:
		item.Index=index;
		item.Name=((Hangar *)object[state])->GetDoor(index)->GetName();
		return item;
	case AscensionTowerState::HangarForRoomSelection:
		item.Index=index;
		{
			Hangar *h=ascension->GetHangar(HANGARTYPETA | HANGARTYPELFMC | HANGARTYPEPORT | HANGARTYPEVLC, index);
			sprintf(text, "%c %s", ascension->GetControlRoom()->GetHangar()==h?'*':' ', h->GetName());
		}
		item.Name=text;
		return item;
	case AscensionTowerState::HangarForPersonSelection:
		item.Index=index;
		{
			Hangar *h=ascension->GetHangar(HANGARTYPETA | HANGARTYPELFMC | HANGARTYPEPORT | HANGARTYPEVLC, index);
			sprintf(text, "%c %s",
				ascension->GetPerson(selectedIndex[AscensionTowerState::PersonControl]).Location->GetHangar()==h?'*':' ',
				h->GetName());
		}
		item.Name=text;
		return item;
	case AscensionTowerState::RoomForPersonSelection:
		item.Index=index;
		{			
			Room *r=((Hangar *)object[state])->GetRoom(index);
			sprintf(text, "%c %s",
				ascension->GetPerson(selectedIndex[AscensionTowerState::PersonControl]).Location==r?'*':' ',
				r->GetName());
		}
		item.Name=text;
		return item;
	case AscensionTowerState::RoomSelection:
		item.Index=index;
		{
			Room *r=((Hangar *)object[state])->GetRoom(index);
			sprintf(text, "%c %s", ascension->GetControlRoom()==r?'*':' ', r->GetName());
		}
		item.Name=text;
		return item;
	case AscensionTowerState::HangarForCraneSelection:
		item.Index=index;
		item.Name=ascension->GetHangar(HANGARTYPETA, index)->GetName();
		return item;
	case AscensionTowerState::CraneList:
		item.Index=index;
		{
			VECTOR3 waypoint=((Crane *)object[state])->GetWaypoint(index);
			if (waypoint.x<0)
			{
				if (waypoint.y<0 && waypoint.z<0)
									sprintf(text, "OUT OF RANGE");
				else switch ((int)waypoint.x)
				{
				case LISTSTOP:		sprintf(text, "STOP"); break;
				case LISTJUMP:		sprintf(text, "JUMP %d", (int)waypoint.y); break;
				case LISTPAUSE:		sprintf(text, "PAUSE %f", waypoint.y); break;
				case LISTGRAPPLE:	sprintf(text, "GRAPPLE"); break;
				case LISTRELEASE:	sprintf(text, "RELEASE"); break;
				case LISTSPEEDS:	sprintf(text, "SPEED %f, CRAWL %f", waypoint.y, waypoint.z); break;
				case LISTEMPTY:		sprintf(text, ""); break;
				default:			sprintf(text, "ILLEGAL OPERATION"); break;
				}
			} else					sprintf(text, "GOTO %6.2f %6.2f %6.2f", waypoint.x, waypoint.y, waypoint.z);			
		}
		item.Name=text;
		return item;
	case AscensionTowerState::TaxiRouteStartSelection:
		item.Index=index;
		{
			Routes *t=ascension->GetTaxiways();
			item.Name=t->GetPoint(index);
			sprintf(text, "%c %s", t->Strobing(item.Name)?'*':' ', item.Name);
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
			sprintf(text, "%c %s", t->On(item.Name)?'*':' ', item.Name);
		}
		item.Name=text;
		return item;
	case AscensionTowerState::Roster:
		item.Index=index;
		{
			Person person=ascension->GetPerson(index);
			if (index>0) sprintf(text, "  %s %s", person.MiscId, person.Name);
			else sprintf(text, "  Add new person...");
		}
		item.Name=text;
		return item;
	case AscensionTowerState::PassengerTransfer:
		item.Index=index;
		{
			Room *r=((Hangar *)object[state])->GetRoom(index);
			VESSEL *v=r->GetDock();
			sprintf(text, "%c %s", v==vessel?'*':v==NULL?' ':'o', r->GetName());
		}
		item.Name=text;
		return item;
	}
	return nullItem;*/
}

AscensionTowerPageInstance GroundPage::Select(int index)
{
	AscensionTowerPage::Select(index);
	switch(selection)
	{
		case 0: return HangarForDoorSelection;
		case 1: return TaxiRouteStartSelection;
		case 2: return HangarForCraneSelection;
		case 3: return PassengerTerminal;
		case 4:
			data->GetPage(PassengerTransfer)->SetDataRoot(ascension->GetNearestHangar(HANGARTYPETA | HANGARTYPELFMC | HANGARTYPEPORT, vessel));
			return PassengerTransfer;
		default: return Undefined;
	}
	/*
	switch(state)
	{
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
		object[AscensionTowerState::DoorSelection]=ascension->GetHangar(HANGARTYPETA | HANGARTYPELL | HANGARTYPELFMC | HANGARTYPELH | HANGARTYPEVLC, index);
		SetState(AscensionTowerState::DoorSelection);		
		break;
	case AscensionTowerState::HangarForRoomSelection:
		index=selectedIndex[state];
		object[AscensionTowerState::RoomSelection]=ascension->GetHangar(HANGARTYPETA | HANGARTYPELFMC | HANGARTYPEPORT | HANGARTYPEVLC, index);
		SetState(AscensionTowerState::RoomSelection);		
		break;
	case AscensionTowerState::HangarForPersonSelection:
		index=selectedIndex[state];
		object[AscensionTowerState::RoomForPersonSelection]=ascension->GetHangar(HANGARTYPETA | HANGARTYPELFMC | HANGARTYPEPORT | HANGARTYPEVLC, index);
		SetState(AscensionTowerState::RoomForPersonSelection);		
		break;
	case AscensionTowerState::RoomForPersonSelection:
		index=ascension->ChangePerson(selectedIndex[AscensionTowerState::PersonControl], PERSON_LOCATION, ((Hangar *)object[state])->GetRoom(selectedIndex[state]));
		if (index<0) break; //TODO: denial sound
		SetState(AscensionTowerState::Roster);
		Select(index);
		break;
	case AscensionTowerState::RoomSelection:		
		ascension->SwitchView(((Hangar *)object[state])->GetRoom(selectedIndex[state]));
		break;
	case AscensionTowerState::DoorSelection:
		object[AscensionTowerState::DoorControl]=((Hangar *)object[state])->GetDoor(selectedIndex[state]);
		SetState(AscensionTowerState::DoorControl);
		break;
	case AscensionTowerState::HangarForCraneSelection:
		object[state]=ascension->GetHangar(HANGARTYPETA, selectedIndex[state]);
		object[AscensionTowerState::CraneControl]=object[AscensionTowerState::CraneList]=object[AscensionTowerState::CraneGrapple]=((TurnAroundHangar *)object[state])->GetCrane();
		SetState(AscensionTowerState::CraneGrapple);
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
			SetState(AscensionTowerState::GroundMenu);
		}
		break;
	case AscensionTowerState::LandingRunwaySelection:
		ascension->GetRunways()->Activate(selectedIndex[state]);
		break;
	case AscensionTowerState::Roster:
		selectedIndex[AscensionTowerState::PersonControl]=selectedIndex[state];
		SetState(AscensionTowerState::PersonControl);
		break;
	case AscensionTowerState::PassengerTransfer:
		Room *r=((Hangar *)object[state])->GetRoom(selectedIndex[state]);
		VESSEL *v=r->GetDock();
		if (v==NULL) ascension->DockVessel(r, vessel);
		else if (v==vessel) ascension->DockVessel(r, NULL);
		break;
	}*/
}

// Return button labels
char *GroundPage::LabelRenderer (int bt)
{
	switch (bt)
	{
		case 6: return "HOM";
		case 7: return "BCK";
		case 8: return "RES";
		default: return AscensionTowerPage::LabelRenderer(bt);
	}
	/*
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
			case 7: return "AUT";
			case 8: return "SPD";			
			case 9: return "DIR";
			case 10: return "MOD";
			case 11: return "BCK";
		}
		return NULL;
	case AscensionTowerState::CraneList:
		switch (bt)
		{
			case 0: return "X+";
			case 1: return "X-";
			case 2: return "Y+";
			case 3: return "Y-";
			case 4: return "Z+";
			case 5: return "Z-";
			case 6: return "STP";
			case 7: return "TEA";
			case 8: return "NXT";
			case 9: return "PRV";
			case 10: return "MOD";
			case 11: return "BCK";
		}
		return NULL;
	case AscensionTowerState::CraneGrapple:
		switch (bt)
		{
			case 0: return "X+";
			case 1: return "X-";
			case 2: return "Y+";
			case 3: return "Y-";
			case 4: return "Z+";
			case 5: return "Z-";
			case 6: return "STP";
			case 7: return "";
			case 8: return "GRP";
			case 9: return "TYP";
			case 10: return "MOD";
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
			case 6: return "HOM";
			case 7: return "BCK";
			case 8: return "RES";
			case 9: return "";
			case 10: return selectedIndex[AscensionTowerState::Roster]>0?"EVA":"";
			case 11: return selectedIndex[AscensionTowerState::Roster]>0?"DEL":"";
		}
		return NULL;
	default:
		int size=GetListSize();
		switch (bt)
		{
			case 6: return state!=AscensionTowerState::BaseSelect && state!=AscensionTowerState::MainMenu?"HOM":"BAS";
			case 7: return state!=AscensionTowerState::BaseSelect && state!=AscensionTowerState::MainMenu?"BCK":"";
			case 8: return state!=AscensionTowerState::BaseSelect && state!=AscensionTowerState::MainMenu?"RES":"";
			case 9: return "";
			case 10: return size>6?"NXT":"";
			case 11: return size>6?"PRV":"";
			default: return size>page[state]*6+bt?" > ":"";
		}
		return NULL;
	}*/
}


// Return button menus
int GroundPage::MenuRenderer (MFDBUTTONMENU *mnu)
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
	/*
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
		{"Toggle auto", "sequence", 'O'},
		{"Set speeds", NULL, 'F'},		
		{"Toggle direct", "command", 'C'},
		{"Switch mode", "to list", 'M'},
		{"Go back", NULL, 'B'}};
	static MFDBUTTONMENU craneListMenu[12] = {
		craneControlMenu[0],
		craneControlMenu[1],
		craneControlMenu[2],
		craneControlMenu[3],
		craneControlMenu[4],
		craneControlMenu[5],
		craneControlMenu[6],
		{"Teach position", NULL, 'T'},
		{"Next list", "entry", 'N'},
		{"Previous list", "entry", 'P'},		
		{"Switch mode", "to cargo", 'M'},
		{"Go back", NULL, 'B'}};
	static MFDBUTTONMENU craneGrappleMenu[12] = {
		craneControlMenu[0],
		craneControlMenu[1],
		craneControlMenu[2],
		craneControlMenu[3],
		craneControlMenu[4],
		craneControlMenu[5],
		craneControlMenu[6],
		{NULL, NULL, 0},
		{"Grapple/Release", "cargo", 'G'},
		{"Toggle cargo", "type", 'T'},		
		{"Switch mode", "to settings", 'M'},
		{"Go back", NULL, 'B'}};
	static MFDBUTTONMENU personMenu[12] = {
		{"Change person", "name", 'N'},
		{personMenu[0].line1, "function", 'F'},
		{personMenu[0].line1, "age", 'A'},
		{personMenu[0].line1, "puls", 'P'},
		{personMenu[0].line1, "weigth", 'W'},
		{personMenu[0].line1, "location", 'L'},
		{"Main menu", NULL, 'H'},
		{"Go back", NULL, 'B'},
		{"Reset to default", "values", 'R'},
		{NULL, NULL, 0},
		{"EVA person", NULL, 'E'},
		{"Remove person", "from roster", 'D'}};

	switch(state)
	{	
	case AscensionTowerState::BaseSelect: target="base"; break;		
	case AscensionTowerState::GroundMenu: 
	case AscensionTowerState::ATCMenu:
	case AscensionTowerState::MainMenu:  target="request"; break;
	case AscensionTowerState::Roster: target="person"; break;
	case AscensionTowerState::HangarForCraneSelection:
	case AscensionTowerState::HangarForRoomSelection:
	case AscensionTowerState::HangarForDoorSelection: target="hangar"; break;
	case AscensionTowerState::DoorSelection: target="door"; break;
	case AscensionTowerState::TaxiRouteStartSelection: target="route start"; break;
	case AscensionTowerState::TaxiRouteEndSelection: target="route end"; break;
	case AscensionTowerState::LandingRunwaySelection: target="runway"; break;
	case AscensionTowerState::RoomForPersonSelection:
	case AscensionTowerState::PassengerTransfer:
	case AscensionTowerState::RoomSelection: target="room"; break;
	case AscensionTowerState::DoorControl: target="command"; break;
	case AscensionTowerState::CraneControl:
		for(int i=0;i<12;i++) mnu[i]=craneControlMenu[i];
		return 12;
	case AscensionTowerState::CraneList:
		for(int i=0;i<12;i++) mnu[i]=craneListMenu[i];
		return 12;
	case AscensionTowerState::CraneGrapple:
		for(int i=0;i<12;i++) mnu[i]=craneGrappleMenu[i];
		return 12;
	case AscensionTowerState::PersonControl:
		for(int i=0;i<12;i++) mnu[i]=personMenu[i];
		if (selectedIndex[AscensionTowerState::Roster]==0)
		{
			mnu[10]=personMenu[9];
			mnu[11]=personMenu[9];
		}
		return 12;
	}
	sprintf(select, "Select %s", target);

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
	mnu[k].line1=(state==AscensionTowerState::BaseSelect || state==AscensionTowerState::MainMenu)?"Scan for bases":"Main menu";
	mnu[k].line2=NULL;
	mnu[k++].selchar='H';
	if (state!=AscensionTowerState::BaseSelect && state!=AscensionTowerState::MainMenu)
	{
		mnu[k].line1="Go back";
		mnu[k].line2=NULL;
		mnu[k++].selchar='B';
		mnu[k].line1="Reset to default";
		mnu[k].line2="values";
		mnu[k++].selchar='R';
	}
	for(;k<10;k++)
	{
		mnu[k].line1=NULL;
		mnu[k].line2=NULL;
		mnu[k].selchar=0;
	}
	if (size>6)
	{
		mnu[k].line1="Switch to";
		mnu[k].line2="next page";
		mnu[k++].selchar='N';
		mnu[k].line1="Switch to";
		mnu[k].line2="previous page";
		mnu[k++].selchar='P';		
	}	
	for(;k<12;k++)
	{
		mnu[k].line1=NULL;
		mnu[k].line2=NULL;
		mnu[k].selchar=0;
	}
	return 12;*/
}

// Handling button presses
AscensionTowerPageInstance GroundPage::ButtonHandler(int bt)
{
	return AscensionTowerPage::ButtonHandler(bt);
	/*
	switch(state)
	{
	case AscensionTowerState::CraneControl:
	case AscensionTowerState::CraneList:
	case AscensionTowerState::CraneGrapple:
		switch(bt)
		{
		case 0: return SetKey(OAPI_KEY_A);
		case 1: return SetKey(OAPI_KEY_D);
		case 2: return SetKey(OAPI_KEY_W);
		case 3: return SetKey(OAPI_KEY_S);
		case 4: return SetKey(OAPI_KEY_Q);
		case 5: return SetKey(OAPI_KEY_E);
		case 6: return SetKey(OAPI_KEY_R);
		case 11: return SetKey(OAPI_KEY_B);
		default:
			switch(state)
			{
				case AscensionTowerState::CraneControl:
					switch(bt)
					{
					case 7: return SetKey(OAPI_KEY_O);
					case 8: return SetKey(OAPI_KEY_F);
					case 9: return SetKey(OAPI_KEY_C);
					case 10: return SetKey(OAPI_KEY_M);
					}
					break;
				case AscensionTowerState::CraneList:
					switch(bt)
					{
					case 7: return SetKey(OAPI_KEY_T);
					case 8: return SetKey(OAPI_KEY_N);
					case 9: return SetKey(OAPI_KEY_P);
					case 10: return SetKey(OAPI_KEY_M);					
					}
					break;
				case AscensionTowerState::CraneGrapple:
					switch(bt)
					{
					case 8: return SetKey(OAPI_KEY_G);
					case 9: return SetKey(OAPI_KEY_T);
					case 10: return SetKey(OAPI_KEY_M);					
					}
					break;
			}
			break;
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
		case 6: return SetKey(OAPI_KEY_H);
		case 7: return SetKey(OAPI_KEY_B);
		case 8: return SetKey(OAPI_KEY_R);
		case 10: return SetKey(OAPI_KEY_E);
		case 11: return SetKey(OAPI_KEY_D);
		}
		break;
*/
}

/*bool ChangePersonData(void *id, char *str, void *usrdata)
{
	AscensionTowerCallbackData *cp=(AscensionTowerCallbackData *)usrdata;
	AscensionTowerData *data=cp->Data;
	int index=data->GetAscension()->ChangePerson(data->GetSelectedIndex(), cp->Flags, str);
	data->SetState(AscensionTowerState::Roster);
	data->Select(index);
	data->GetMfd()->InvalidateDisplay();
	return true;
}

bool EditPosition(void *id, char *str, void *usrdata)
{
	AscensionTowerCallbackData *cp=(AscensionTowerCallbackData *)usrdata;
	AscensionTowerData *data=cp->Data;	
	char *end=str+strlen(str);
	for(;str!=end;str++) if (*str>32) break;
	switch (*str)
	{
	case 0:
		((Crane*)data->GetObject())->SetWaypoint(data->GetSelectedIndex(), _V(LISTEMPTY, 0, 0));
		break;
	case 's':
		str++;
		end=str+strlen(str);
		for(;str!=end;str++) if (*str>32) break;
		if (str==end) ((Crane*)data->GetObject())->SetWaypoint(data->GetSelectedIndex(), _V(LISTSTOP, 0, 0));
		else
		{
			float speed, crawl;
			if (sscanf(str, "%f %f", &speed, &crawl)!=2) return false;
			((Crane*)data->GetObject())->SetWaypoint(data->GetSelectedIndex(), _V(LISTSPEEDS, speed, crawl));
		}
		break;
	case 'j':
		{
			unsigned int dest;
			if (sscanf(++str, "%u", &dest)!=1) return false;
			((Crane*)data->GetObject())->SetWaypoint(data->GetSelectedIndex(), _V(LISTJUMP, dest, 0));
		}
		break;
	case 'p':
		{
			float pause;
			if (sscanf(++str, "%f", &pause)!=1) return false;
			((Crane*)data->GetObject())->SetWaypoint(data->GetSelectedIndex(), _V(LISTPAUSE, pause, 0));
		}
		break;
	case 'g':
		((Crane*)data->GetObject())->SetWaypoint(data->GetSelectedIndex(), _V(LISTGRAPPLE, 0, 0));
		break;
	case 'r':
		((Crane*)data->GetObject())->SetWaypoint(data->GetSelectedIndex(), _V(LISTRELEASE, 0, 0));
		break;
	default:
		{
			float x,y,z;
			if (sscanf(str, "%f %f %f", &x,&y,&z)!=3) return false;
			Crane *crane=(Crane*)data->GetObject();
			VECTOR3 len=crane->GetLength();
			if (x<0 || y<0 || z<0 || x>len.x || y>len.y || z>len.z) return false;
			crane->SetWaypoint(data->GetSelectedIndex(), _V(x, y, z));
		}
	}
	return true;
}*/

// Handling shortcut keys
AscensionTowerPageInstance GroundPage::KeyHandler(DWORD key)
{	
	switch(key)
	{	
	case OAPI_KEY_H:
	case OAPI_KEY_B:
		return MainMenu;
	default:
		return AscensionTowerPage::KeyHandler(key);
	}

	/*int size=GetListSize();
	int pages=(size+5)/6;
	switch(state)
	{
	case AscensionTowerState::CraneControl:
	case AscensionTowerState::CraneList:
	case AscensionTowerState::CraneGrapple:
		{
			Crane *crane=(Crane*)GetObject();
			VECTOR3 pos=crane->GetPosition();
			if (crane->GetMode()<=CRANEMANUAL) switch(key)
			{
			case OAPI_KEY_A:
				pos.x+=step[state];
				crane->SetPosition(pos);
				break;
			case OAPI_KEY_D:
				pos.x-=step[state];
				crane->SetPosition(pos);
				break;
			case OAPI_KEY_W:
				pos.y+=step[state];
				crane->SetPosition(pos);
				break;
			case OAPI_KEY_S:
				pos.y-=step[state];
				crane->SetPosition(pos);
				break;
			case OAPI_KEY_Q:
				pos.z+=step[state];
				crane->SetPosition(pos);
				break;
			case OAPI_KEY_E:
				pos.z-=step[state];
				crane->SetPosition(pos);
				break;
			case OAPI_KEY_R:
				step[state]*=10;
				if (step[state]>100) step[state]=0.01;
				step[AscensionTowerState::CraneControl]=step[AscensionTowerState::CraneGrapple]=step[AscensionTowerState::CraneList]=step[state];
				break;
			case OAPI_KEY_B:
				Back();
				break;
			default:
				result=false;
				break;
			}
			else
			{
				//TODO: invalid sound
				if (key==OAPI_KEY_B) Back();
				else result=false;
			}
			if (result) return true; //Check for manual mode key usage and return here already
			result=true; //Reset result to true for subsequent crane mode keys
			switch(state)
			{
			case AscensionTowerState::CraneControl:
				switch(key)
				{		
				case OAPI_KEY_O:
					crane->SetMode(crane->GetMode()>CRANEMANUAL?CRANEMANUAL:selectedIndex[AscensionTowerState::CraneList]);						
					break;
				case OAPI_KEY_F:
					{
						VECTOR3 speed=crane->GetSpeed();
						VECTOR3 crawl=crane->GetCrawl();
						sprintf(buffer, "%6.2f[%6.2f], %6.2f[%6.2f], %6.2f[%6.2f]", speed.x, crawl.x, speed.y, crawl.y, speed.z, crawl.z);
					}
					oapiOpenInputBox("Edit speeds (long,short,reel with fast[slow]):", EditSpeed, buffer, 52, (void *)&changePerson);
					break;
				case OAPI_KEY_C:
					crane->SetMode(crane->GetMode()==CRANEDIRECT?CRANEMANUAL:CRANEDIRECT);
					break;
				case OAPI_KEY_M:
					SetState(AscensionTowerState::CraneList);
					break;					
				default:
					result=false;
					break;
				}
				break;

			case AscensionTowerState::CraneList:
				pages=(size+9)/10;
				selectedIndex[state]=page[state]*10+selection[state];
				switch(key)
				{		
				case OAPI_KEY_T:
					{						
						VECTOR3 waypoint=crane->GetWaypoint(selectedIndex[state]);
						sprintf(buffer, "%6.2f %6.2f %6.2f", pos.x, pos.y, pos.z);
						if (waypoint.x<0)
						{
							if (waypoint.y<0 && waypoint.z<0) break;
							else switch ((int)waypoint.x)
							{
							case LISTSTOP:		sprintf(buffer, "s"); break;
							case LISTJUMP:		sprintf(buffer, "j%d", (int)waypoint.y); break;
							case LISTPAUSE:		sprintf(buffer, "p%f", waypoint.y); break;
							case LISTGRAPPLE:	sprintf(buffer, "g"); break;
							case LISTRELEASE:	sprintf(buffer, "r"); break;
							case LISTSPEEDS:	sprintf(buffer, "s%f %f", waypoint.y, waypoint.z); break;
							}
						}
					}
					oapiOpenInputBox("Edit list entry ( * * *|s|j*|p*|g|r|s* *|empty ):", EditPosition, buffer, 26, (void *)&changePerson);
					break;
				case OAPI_KEY_N:
					if (selection[state]<min(size-page[state]*10, 10)-1) selection[state]++;
					else
					{
						if (page[state]<pages-1) page[state]++;
						else page[state]=0;
						selection[state]=0;
					}
					break;
				case OAPI_KEY_P:
					if (selection[state]>0) selection[state]--;
					else
					{
						if (page[state]>0) page[state]--;
						else page[state]=pages-1;
						selection[state]=min(size-page[state]*10, 10)-1;
					}
					break;
				case OAPI_KEY_M:
					SetState(AscensionTowerState::CraneGrapple);
					break;
				default:
					result=false;
					break;
				}
				break;

			case AscensionTowerState::CraneGrapple:
				switch(key)
				{
				case OAPI_KEY_G:
					break;
				case OAPI_KEY_T:			
					break;
				case OAPI_KEY_M:
					SetState(AscensionTowerState::CraneControl);
					break;
				default:
					result=false;
					break;
				}
				break;

			default:
				result=false;
				break;
			}
			return result;
		}

	case AscensionTowerState::PersonControl:
		switch(key)
		{
		case OAPI_KEY_N:
			changePerson.Flags=PERSON_NAME;
			oapiOpenInputBox("Change person name:", ChangePersonData, strncpy(buffer, ascension->GetPerson(selectedIndex[AscensionTowerState::Roster]).Name, BUFFERLEN), 26, (void *)&changePerson);
			break;
		case OAPI_KEY_F:
			changePerson.Flags=PERSON_MISCID;
			oapiOpenInputBox("Change function (Crew,Capt,Sek,Vip,Sci,Doc,Tech):", ChangePersonData, strncpy(buffer, ascension->GetPerson(selectedIndex[AscensionTowerState::Roster]).MiscId, BUFFERLEN), 26, (void *)&changePerson);
			break;
		case OAPI_KEY_A:
			changePerson.Flags=PERSON_AGE;
			sprintf(buffer, "%d", ascension->GetPerson(selectedIndex[AscensionTowerState::Roster]).Age);
			oapiOpenInputBox("Change person age:", ChangePersonData, buffer, 26, (void *)&changePerson);
			break;
		case OAPI_KEY_P:
			changePerson.Flags=PERSON_PULS;
			sprintf(buffer, "%d", ascension->GetPerson(selectedIndex[AscensionTowerState::Roster]).Puls);
			oapiOpenInputBox("Change person puls:", ChangePersonData, buffer, 26, (void *)&changePerson);
			break;
		case OAPI_KEY_W:
			changePerson.Flags=PERSON_WEIGHT;
			sprintf(buffer, "%d", ascension->GetPerson(selectedIndex[AscensionTowerState::Roster]).Weight);
			oapiOpenInputBox("Change person weight:", ChangePersonData, buffer, 26, (void *)&changePerson);
			break;
		case OAPI_KEY_L:
			SetState(AscensionTowerState::HangarForPersonSelection);
			break;
		case OAPI_KEY_E:
			if (selectedIndex[AscensionTowerState::Roster]==0) return false;
			switch (ascension->ChangePerson(selectedIndex[AscensionTowerState::Roster], PERSON_EVA))
			{
			case ERROR_DOCKEDSHIP_DONOT_USE_UMMU:
			case ERROR_DOCKED_SHIP_HAVE_AIRLOCK_CLOSED:
			case ERROR_DOCKED_SHIP_IS_FULL:
				//TODO: denial sound				
				break;
			case EVA_OK:
				SetState(AscensionTowerState::Roster);
				break;
			}			
			break;
		case OAPI_KEY_D:
			if (selectedIndex[AscensionTowerState::Roster]==0) return false;
			ascension->ChangePerson(selectedIndex[AscensionTowerState::Roster], PERSON_DELETE);
			SetState(AscensionTowerState::Roster);
			break;
		case OAPI_KEY_B:
			Back();
			break;
		case OAPI_KEY_H:
			SetState(AscensionTowerState::MainMenu);
			break;
		case OAPI_KEY_R:
			//Reset();
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
		if (size<7) return Undefined;
		if (page<pages-1) page++;
		else page=0;
		selection=0;
		return NoChange;
	case OAPI_KEY_P://Previous page
		if (size<7) return Undefined;
		if (page>0) page--;
		else page=pages-1;
		selection=min(size-page*6, 6)-1;
		return NoChange;
	case OAPI_KEY_B://Go back
		if (state!=AscensionTowerState::BaseSelect && state!=AscensionTowerState::MainMenu)	Back();
		break;
	case OAPI_KEY_H://Main menu/Scan for bases
		//if (state==AscensionTowerState::BaseSelect || state==AscensionTowerState::MainMenu)
		return BaseSelect;
		//else SetState(AscensionTowerState::MainMenu);
	case OAPI_KEY_R://Reset to default
		//if (state!=AscensionTowerState::BaseSelect && state!=AscensionTowerState::MainMenu) Reset();
		return NoChange;
	default:
		if (key<OAPI_KEY_1 || key>OAPI_KEY_6) return Undefined;
		{
			int bt=key-OAPI_KEY_1;
			if (bt>=min(size-page*6, 6)) return Undefined;
			selection=bt;
			return Select();
		}		
	}
	return Undefined;*/
}

char *GroundPage::GetTitle()
{
	/*static char *tower=" Tower";
	static char *ground=" Ground";
	static char *atc=" ATC";
	static char *rooster=" Roster";
	static char title[57];
	switch(state)
	{*/
	return "Ascension Tower";
	/*case AscensionTowerState::GroundMenu:
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
	case AscensionTowerState::CraneList:
	case AscensionTowerState::CraneGrapple:
	case AscensionTowerState::PassengerTransfer:
		return GetNameSafeTitle(title, ground);	
	case AscensionTowerState::ATCMenu:
	case AscensionTowerState::Bearing:
	case AscensionTowerState::LandingRunwaySelection:
	case AscensionTowerState::Launch:
		return GetNameSafeTitle(title, atc);	
		break;
	case AscensionTowerState::HangarForPersonSelection:
	case AscensionTowerState::RoomForPersonSelection:
	case AscensionTowerState::Roster:
	case AscensionTowerState::PersonControl:
		return GetNameSafeTitle(title, rooster);
		break;
	case AscensionTowerState::MainMenu:
	case AscensionTowerState::HangarForRoomSelection:
	case AscensionTowerState::RoomSelection:
	default:		
		return GetNameSafeTitle(title, tower);	
	}*/	
}

char *GroundPage::GetSubtitle()
{
	/*static char subtitle[57];

	switch(state)
	{*/
	return "Select request";
	/*
	case AscensionTowerState::GroundMenu: return "Select ground request";
	case AscensionTowerState::ATCMenu: return "Select ATC request";	
	case AscensionTowerState::Roster: return "Select Person";
	case AscensionTowerState::PersonControl: return selectedIndex[AscensionTowerState::Roster]>0?"Person Information":"Add new person";
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
	case AscensionTowerState::PassengerTransfer: return "Select Destination for PAX";
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
		sprintf(subtitle, "%s -> Crane -> Settings",
			((Hangar *)object[AscensionTowerState::HangarForCraneSelection])->GetName());
		return subtitle;
	case AscensionTowerState::CraneList:
		sprintf(subtitle, "%s -> Crane -> List",
			((Hangar *)object[AscensionTowerState::HangarForCraneSelection])->GetName());
		return subtitle;
	case AscensionTowerState::CraneGrapple:
		sprintf(subtitle, "%s -> Crane -> Cargo",
			((Hangar *)object[AscensionTowerState::HangarForCraneSelection])->GetName());
		return subtitle;
	case AscensionTowerState::HangarForPersonSelection:
		sprintf(subtitle, "%s -> Hangar Location",
			ascension->GetPerson(selectedIndex[AscensionTowerState::PersonControl]).Name);
		return subtitle;
	case AscensionTowerState::RoomForPersonSelection:
		sprintf(subtitle, "%s -> %s -> Room Location",
			ascension->GetPerson(selectedIndex[AscensionTowerState::PersonControl]).Name,
			((Hangar *)object[state])->GetName());
		return subtitle;
	}
	return "";*/
}

/*void AscensionTowerData::Back()
{
	switch(state)
	{
	case AscensionTowerState::MainMenu:
	case AscensionTowerState::BaseSelect: SetState(AscensionTowerState::BaseSelect);break;	
	case AscensionTowerState::GroundMenu: SetState(AscensionTowerState::MainMenu);break;
	case AscensionTowerState::ATCMenu: SetState(AscensionTowerState::MainMenu);break;
	case AscensionTowerState::Roster: SetState(AscensionTowerState::MainMenu);break;
	case AscensionTowerState::PersonControl: SetState(AscensionTowerState::Roster);break;
	case AscensionTowerState::RoomForPersonSelection: SetState(AscensionTowerState::HangarForPersonSelection);break;
	case AscensionTowerState::HangarForPersonSelection: SetState(AscensionTowerState::PersonControl);break;
	case AscensionTowerState::HangarForDoorSelection: SetState(AscensionTowerState::GroundMenu);break;
	case AscensionTowerState::DoorSelection: SetState(AscensionTowerState::HangarForDoorSelection);break;
	case AscensionTowerState::DoorControl: SetState(AscensionTowerState::DoorSelection);break;
	case AscensionTowerState::TaxiRouteStartSelection: SetState(AscensionTowerState::GroundMenu);break;
	case AscensionTowerState::TaxiRouteEndSelection: SetState(AscensionTowerState::TaxiRouteStartSelection);break;
	case AscensionTowerState::HangarForCraneSelection: SetState(AscensionTowerState::GroundMenu);break;
	case AscensionTowerState::CraneControl:
	case AscensionTowerState::CraneList:
	case AscensionTowerState::CraneGrapple:
		SetState(AscensionTowerState::HangarForCraneSelection);
		break;
	case AscensionTowerState::PassengerTerminal: SetState(AscensionTowerState::GroundMenu);break;
	case AscensionTowerState::PassengerTransfer: SetState(AscensionTowerState::GroundMenu);break;
	case AscensionTowerState::Fueling: SetState(AscensionTowerState::PassengerTerminal);break;
	case AscensionTowerState::LaunchPrepare: SetState(AscensionTowerState::Fueling);break;
	case AscensionTowerState::Bearing: SetState(AscensionTowerState::ATCMenu);break;
	case AscensionTowerState::LandingRunwaySelection: SetState(AscensionTowerState::ATCMenu);break;
	case AscensionTowerState::Launch: SetState(AscensionTowerState::ATCMenu);break;
	case AscensionTowerState::HangarForRoomSelection: SetState(AscensionTowerState::MainMenu);break;
	case AscensionTowerState::RoomSelection: SetState(AscensionTowerState::HangarForRoomSelection);break;
	}
}*/