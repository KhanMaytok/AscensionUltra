#pragma once
#include <vector>
#include "AscensionUltra.h"
#include "orbitersdk.h"

#define BUFFERLEN 256

class AscensionTowerData
{
public:
	AscensionTowerData(MFD *mfd, VESSEL *vessel);
	~AscensionTowerData(void);
	
	double GetStep();
	int GetPage();
	int GetSelection();
	AscensionTowerState GetState();
	void SetState(AscensionTowerState state);
	void SetPage(int page);
	void *GetObject();
	int GetSelectedIndex();
	MFD *GetMfd();
private:	
	VESSEL *vessel;
	char buffer[BUFFERLEN+1];
	MFD *mfd;
	int page[STATES], selectedIndex[STATES], selection[STATES];
	double step[STATES];
	void *object[STATES];
};
