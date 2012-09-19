#pragma once
#include "AscensionTowerData.h"

#define WRITEMFD_HALFLINES		0x01
#define WRITEMFD_RIGHTALINED	0x04
#define WRITEMFD_HIGHLIGHTED	0x08

#define HIGHLIGHTED(flags)	(flags & WRITEMFD_HIGHLIGHTED)>0
#define HALFLINES(flags)	(flags & WRITEMFD_HALFLINES)>0
#define RIGHTALINED(flags)	(flags & WRITEMFD_RIGHTALINED)>0

typedef enum AscensionTowerPageInstance
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
	CraneList,
	CraneGrapple,
	Reset,
	NoChange,
	Undefined,
};

struct AscensionTowerListPair
{
	UINT Index;
	char *Name;
};

extern HBRUSH g_Bar;
extern COLORREF g_MiddleGreen;
static int AT_BUTTON[6], AT_BUTTONDOUBLED[10];

class AscensionTowerPage
{
public:
	AscensionTowerPage(AscensionTowerData *data);
	~AscensionTowerPage(void);

	virtual void RenderPage();
	virtual int GetListSize();
	virtual AscensionTowerListPair GetListItem(int index);
	virtual AscensionTowerPageInstance Select(int index=-1);
	virtual char *GetButtonLabel (int bt);
	virtual int GetButtonMenu (MFDBUTTONMENU *mnu);
	virtual AscensionTowerPageInstance SetButton(int bt);
	virtual AscensionTowerPageInstance SetKey(DWORD key);
	virtual char *GetTitle();
	virtual char *GetSubtitle();
	
protected:
	AscensionTowerData *data;
	float width, height;
	int mfdWidth, mfdHeight;
	HDC hDC;
	int selectedIndex, selection, page;
	char *GetNameSafeTitle(char *title, char *trailer);
	void WriteMFD(char *text, int line=-1, int column=-1, int flags=0);
};
