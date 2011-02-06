#pragma once
#include <vector>
#include "AscensionUltra.h"
#include "orbitersdk.h"

#define STATES 24
#define BUFFERLEN 256

typedef enum AscensionTowerState
{
	BaseSelect,
	MainMenu,
	GroundMenu,
	ATCMenu,
	HangarForRoomSelection,
	RoomSelection,
	HangarForDoorSelection,
	DoorSelection,
	TaxiRouteStartSelection,
	TaxiRouteEndSelection,
	HangarForCraneSelection,
	PassengerTerminal,
	Fueling,
	LaunchPrepare,
	Bearing,
	LandingRunwaySelection,
	Launch,
	DoorControl,
	CraneControl,
	Roster,
	PersonControl,
	RoomForPersonSelection,
	HangarForPersonSelection,
	PassengerTransfer,
};

typedef struct AscensionTowerListPair
{
	UINT Index;
	char *Name;
};

class AscensionTowerData;

typedef struct AscensionTowerChangePerson
{
	int Flags;	
	AscensionTowerData *Data;
};

class AscensionTowerData
{
public:
	AscensionTowerData(MFD *mfd, VESSEL *vessel);
	~AscensionTowerData(void);
	AscensionUltra *GetAscension();	
	int GetPage();
	int GetListSize();
	AscensionTowerListPair GetListItem(int index);
	int GetSelection();
	void Select(int index=-1);
	AscensionTowerState GetState();
	char *GetButtonLabel (int bt);
	int GetButtonMenu (MFDBUTTONMENU *mnu);
	bool SetButton(int bt);
	bool SetKey(DWORD key);
	void SetState(AscensionTowerState state);
	void SetPage(int page);
	char *GetTitle();
	char *GetSubtitle();
	void *GetObject();
	int GetSelectedIndex();
	MFD *GetMfd();
private:	
	VESSEL *vessel;
	OBJHANDLE ascensionHandle;
	char *ascensionName, buffer[BUFFERLEN+1];
	AscensionUltra *ascension;
	std::vector<AscensionTowerListPair> scanList;
	AscensionTowerState state;
	AscensionTowerChangePerson changePerson;
	MFD *mfd;
	int page[STATES], selectedIndex[STATES], selection[STATES];
	void *object[STATES];
	void Scan();
	void SetAscension(int index);	
	void Back();
	char *GetNameSafeTitle(char *title, char *trailer);
};
