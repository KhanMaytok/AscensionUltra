#pragma once
#include <vector>
#include "AscensionUltra.h"
#include "orbitersdk.h"

#define STATES 17

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
	TaxiRouteSelection,
	HangarForCraneSelection,
	PassengerTerminal,
	Fueling,
	LaunchTunnel,
	Bearing,
	LandingRunwaySelection,
	Launch,
	DoorControl,
};

typedef struct AscensionTowerListPair
{
	UINT Index;
	char *Name;
};

class AscensionTowerData
{
public:
	AscensionTowerData(void);
	~AscensionTowerData(void);
	AscensionUltra *GetAscension();	
	int GetPage();
	int GetListSize();
	AscensionTowerListPair GetListItem(int index);
	int GetSelection();
	AscensionTowerState GetState();
	char *GetButtonLabel (int bt);
	int GetButtonMenu (MFDBUTTONMENU *mnu);
	bool SetButton(int bt);
	bool SetKey(DWORD key);
	void SetState(AscensionTowerState state);
	void SetPage(int page);
	char *GetTitle();
	char *GetSubtitle();
private:	
	OBJHANDLE ascensionHandle;
	char *ascensionName;
	AscensionUltra *ascension;
	std::vector<AscensionTowerListPair> scanList;
	AscensionTowerState state;
	int page[STATES], selectedIndex[STATES], selection[STATES];
	void Scan();
	void SetAscension(int index);
	void Select();
	void Back();
	char *GetNameSafeTitle(char *title, char *trailer);
};
