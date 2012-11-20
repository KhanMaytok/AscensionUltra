#pragma once
#include "AscensionTowerPage.h"
#pragma warning(disable : 4482)

class TemplatePage: public AscensionTowerPage
{

public:

	TemplatePage(AscensionTowerData *data):AscensionTowerPage(data){}

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
		}*/
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
		}*/		
	}

	AscensionTowerPageInstance ButtonHandler(int bt)
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
			return result;*/
	}

	char *GetTitle()
	{
		return GetNameSafeTitle("Tower");
		/*
		case AscensionTowerState::PassengerTerminal:
		case AscensionTowerState::Fueling:
		case AscensionTowerState::LaunchPrepare:
		case AscensionTowerState::DoorControl:
		case AscensionTowerState::CraneControl:
		case AscensionTowerState::CraneList:
		case AscensionTowerState::CraneGrapple:
		return GetNameSafeTitle(title, ground);	
		case AscensionTowerState::Bearing:
		case AscensionTowerState::Launch:
			return GetNameSafeTitle(title, atc);	
			break;
		case AscensionTowerState::PersonControl:
			return GetNameSafeTitle(title, rooster);
			break;
		}*/	
	}

	char *GetSubtitle()
	{
		//static char subtitle[57];
		return "Empty page";
		/*
		case AscensionTowerState::PersonControl: return selectedIndex[AscensionTowerState::Roster]>0?"Person Information":"Add new person";
		case AscensionTowerState::PassengerTerminal: return "Passenger Terminal";	
		case AscensionTowerState::Fueling: return "Fueling";	
		case AscensionTowerState::LaunchPrepare: return "Launch Tunnel";	
		case AscensionTowerState::Bearing: return "Bearing";	
		case AscensionTowerState::Launch: return "Request Launch Clearance";	
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
		}*/
	}

	int GetListSize()
	{
		return 0;
		/*switch(state)
		{
		case AscensionTowerState::DoorControl: return 3;
		case AscensionTowerState::CraneList: return ((Crane *)object[state])->GetWaypoints();
		}*/
	}

	AscensionTowerListPair GetListItem(int index)
	{
		static AscensionTowerListPair nullitem={0,NULL};
		return nullitem;
		
		/*static AscensionTowerListPair doorMenu[3]={{0," Open"},{1," Close"},{2," Stop"}};	
		static char text[57];
		AscensionTowerListPair item;
		
		switch(state)
		{
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
		}*/
	}

	AscensionTowerPageInstance Select(int index=-1)
	{
		return Undefined;
		/*
		switch(state)
		{
		case AscensionTowerState::DoorControl:
			switch(selection[state])
			{
			case 0: ((Door *)object[state])->Open(); break;
			case 1: ((Door *)object[state])->Close(); break;
			case 2: ((Door *)object[state])->Stop(); break;
			}		
			break;
		}*/
	}

};



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