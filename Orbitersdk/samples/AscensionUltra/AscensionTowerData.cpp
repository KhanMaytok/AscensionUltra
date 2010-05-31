#include "AscensionTowerData.h"

AscensionTowerData::AscensionTowerData(void)
{
	ascensionHandle=NULL;
	ascensionName=NULL;
	ascension=NULL;
	for(int i=0;i<10;i++)
	{
		page[i]=0;
		selection[i]=0;
	}
	state=AscensionTowerState::MainMenu;
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
			if (detected>=0) return NULL; //Name is not unique
			detected=i->Index;			
		}
	}
	if (detected<0)
	{
		if (scanList.size()!=1) return NULL;
		detected=scanList.begin()->Index;
	}
	SetAscension(detected);	
	return ascension;
}

int AscensionTowerData::GetPage(){return page[state];}
void AscensionTowerData::SetPage(int page){this->page[state]=page;}

void AscensionTowerData::SetAscension(int index)
{
	ascension=NULL;
	if (index<0 || index>=oapiGetVesselCount()) return Scan();
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
	for (int i=0;i<k;i++)
	{
		VESSEL *vessel=oapiGetVesselInterface(oapiGetVesselByIndex(i));	
		if (strcmp(vessel->GetClassName(), "AscensionUltra")==0)
		{
			char *source=vessel->GetName();
			char *target=new char[strlen(source)+1];
			strcpy(target, source);
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
	case AscensionTowerState::MainMenu: return 3;
	case AscensionTowerState::GroundMenu: return 4;
	case AscensionTowerState::ATCMenu: return 3;
	}
	return 0;
}

bool AscensionTowerData::StartList(int index)
{
	listIter=index;
	return ListEnd();	
}

bool AscensionTowerData::NextList()
{
	listIter++;
	return ListEnd();
}

bool AscensionTowerData::ListEnd()
{
	switch(state)
	{
	case AscensionTowerState::BaseSelect: return listIter<scanList.size();
	case AscensionTowerState::MainMenu: return listIter<3;
	case AscensionTowerState::GroundMenu: return listIter<4;
	case AscensionTowerState::ATCMenu: return listIter<3;
	}
	return false;
}

AscensionTowerListPair AscensionTowerData::GetListItem()
{
	static AscensionTowerListPair mainMenu[3]={{0,"1. Request Ground Operation"},{1,"2. Air Traffic Control"},{2,"3. Control Rooms"}};
	static AscensionTowerListPair groundMenu[4]={{0,"1. Request Roll-in/Roll-out"},{1,"2. Request Taxi"},{2,"3. Request Cargo Control"},{3,"4. Request Launch"}};
	static AscensionTowerListPair atcMenu[3]={{0,"1. Request Bearing"},{1,"2. Request Clearance to Land"},{2,"3. Request Launch Clearance"}};
	static AscensionTowerListPair nullItem={0,""};
	switch(state)
	{
	case AscensionTowerState::BaseSelect: return scanList[listIter];
	case AscensionTowerState::MainMenu: return mainMenu[listIter];
	case AscensionTowerState::GroundMenu: return groundMenu[listIter];
	case AscensionTowerState::ATCMenu: return atcMenu[listIter];
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
	switch(state)
	{
	case AscensionTowerState::BaseSelect:		
		SetAscension(scanList[page[0]*6+selection[0]].Index);
		SetState(AscensionTowerState::MainMenu);
		break;	
	case AscensionTowerState::MainMenu:
		switch(selection[state])
		{
		case 0: SetState(AscensionTowerState::GroundMenu); break;
		case 1: SetState(AscensionTowerState::ATCMenu); break;
		case 2: SetState(AscensionTowerState::HangarForRoomSelection); break;
		}		
		break;
	case AscensionTowerState::GroundMenu:
		switch(selection[state])
		{
		case 0: SetState(AscensionTowerState::HangarForDoorSelection); break;
		case 1: SetState(AscensionTowerState::TaxiRouteSelection); break;
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
	}
}

// Return button labels
char *AscensionTowerData::GetButtonLabel (int bt)
{
	int size=GetListSize();
	switch (bt)
	{
		case 6: return size>0?"SEL":"";
		case 7: return size>1?"UP":"";
		case 8: return size>1?"DWN":"";
		case 9: return size>6?"NXT":"";
		case 10: return size>6?"PRV":"";
		case 11: return (state==AscensionTowerState::BaseSelect || state==AscensionTowerState::MainMenu)?"SCN":"CNL";
		default: return size>page[state]*6+bt?" > ":"";
	}
	return NULL;
}

// Return button menus
int AscensionTowerData::GetButtonMenu (MFDBUTTONMENU *mnu)
{	
	static char select[20];
	static char marked[20];
	static char *target;

	switch(state)
	{	
	case AscensionTowerState::BaseSelect: target="base"; break;		
	case AscensionTowerState::GroundMenu: 
	case AscensionTowerState::ATCMenu:
	case AscensionTowerState::MainMenu:  target="request"; break;
	case AscensionTowerState::HangarForCraneSelection:
	case AscensionTowerState::HangarForRoomSelection:
	case AscensionTowerState::HangarForDoorSelection: target="hangar"; break;
	case AscensionTowerState::DoorSelection: target="door"; break;
	case AscensionTowerState::TaxiRouteSelection: target="route"; break;
	case AscensionTowerState::LandingRunwaySelection: target="runway"; break;
	case AscensionTowerState::RoomSelection: target="room"; break;
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
		mnu[7].line1="Move marker up";
		mnu[7].line2=NULL;
		mnu[7].selchar='U';
		mnu[8].line1="Move marker down";
		mnu[8].line2=NULL;
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
		mnu[9].line1="Switch to next";
		mnu[9].line2="page";
		mnu[9].selchar='N';
		mnu[10].line1="Switch to previous";
		mnu[10].line2="page";
		mnu[10].selchar='P';
	}
	else for(int i=9;i<11;i++)
	{
		mnu[i].line1=NULL;
		mnu[i].line2=NULL;
		mnu[i].selchar=0;
	}

	mnu[11].line1=(state==AscensionTowerState::BaseSelect || state==AscensionTowerState::MainMenu)?"Scan for bases":"Cancel command";
	mnu[11].line2=NULL;
	mnu[11].selchar='C';
	
	return 12;
}

// Handling button presses
bool AscensionTowerData::SetButton(int bt)
{
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
	case 11: return SetKey(OAPI_KEY_C);	
	}		
	return false;
}

// Handling shortcut keys
bool AscensionTowerData::SetKey(DWORD key)
{	
	bool result=true;
	int size=GetListSize();
	int pages=(size+5)/6;
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
	case OAPI_KEY_C://Scan for changes
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

char *AscensionTowerData::GetTitle()
{
	static char *label="Tower";
	static char *ground="Ground";
	static char *atc="ATC";
	static char title[40];
	switch(state)
	{
	case AscensionTowerState::BaseSelect:
		sprintf(title, "Ascension %s", label);
		break;
	case AscensionTowerState::GroundMenu:
	case AscensionTowerState::HangarForDoorSelection:
	case AscensionTowerState::DoorSelection:
	case AscensionTowerState::TaxiRouteSelection:
	case AscensionTowerState::HangarForCraneSelection:
	case AscensionTowerState::PassengerTerminal:
	case AscensionTowerState::Fueling:
	case AscensionTowerState::LaunchTunnel:
		sprintf_s(title, 40, "%s Ground", ascension->GetName());
		break;
	case AscensionTowerState::ATCMenu:
	case AscensionTowerState::Bearing:
	case AscensionTowerState::LandingRunwaySelection:
	case AscensionTowerState::Launch:
		sprintf_s(title, 40, "%s ATC", ascension->GetName());
		break;
	case AscensionTowerState::MainMenu:
	case AscensionTowerState::HangarForRoomSelection:
	case AscensionTowerState::RoomSelection:
	default:
		sprintf_s(title, 40, "%s %s", ascension->GetName(), label);
		break;
	}
	return title;
}

char *AscensionTowerData::GetSubtitle()
{
	switch(state)
	{
	case AscensionTowerState::BaseSelect: return scanList.size()>0?"Select base":"";
	case AscensionTowerState::MainMenu: return "Select request";
	case AscensionTowerState::GroundMenu: return "Select ground request";
	case AscensionTowerState::ATCMenu: return "Select ATC request";	
	case AscensionTowerState::HangarForDoorSelection: return "Select Hangar for Roll-in/Roll-out";	
	case AscensionTowerState::DoorSelection: return "Select Door for Roll-in/Roll-out";	
	case AscensionTowerState::TaxiRouteSelection: return "Select Taxi Route";	
	case AscensionTowerState::HangarForCraneSelection: return "Select Hangar for Cargo";	
	case AscensionTowerState::PassengerTerminal: return "Passenger Terminal";	
	case AscensionTowerState::Fueling: return "Fueling";	
	case AscensionTowerState::LaunchTunnel: return "Launch Tunnel";	
	case AscensionTowerState::Bearing: return "Bearing";	
	case AscensionTowerState::LandingRunwaySelection: return "Select Runway for Landing";	
	case AscensionTowerState::Launch: return "Request Launch Clearance";	
	case AscensionTowerState::HangarForRoomSelection: return "Select Hangar for Control Room";	
	case AscensionTowerState::RoomSelection: return "Select Control Room";	
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
	case AscensionTowerState::HangarForDoorSelection: SetState(AscensionTowerState::GroundMenu);break;
	case AscensionTowerState::DoorSelection: SetState(AscensionTowerState::HangarForDoorSelection);break;
	case AscensionTowerState::TaxiRouteSelection: SetState(AscensionTowerState::GroundMenu);break;
	case AscensionTowerState::HangarForCraneSelection: SetState(AscensionTowerState::GroundMenu);break;
	case AscensionTowerState::PassengerTerminal: SetState(AscensionTowerState::GroundMenu);break;
	case AscensionTowerState::Fueling: SetState(AscensionTowerState::PassengerTerminal);break;
	case AscensionTowerState::LaunchTunnel: SetState(AscensionTowerState::Fueling);break;
	case AscensionTowerState::Bearing: SetState(AscensionTowerState::ATCMenu);break;
	case AscensionTowerState::LandingRunwaySelection: SetState(AscensionTowerState::ATCMenu);break;
	case AscensionTowerState::Launch: SetState(AscensionTowerState::ATCMenu);break;
	case AscensionTowerState::HangarForRoomSelection: SetState(AscensionTowerState::MainMenu);break;
	case AscensionTowerState::RoomSelection: SetState(AscensionTowerState::HangarForRoomSelection);break;
	}
}