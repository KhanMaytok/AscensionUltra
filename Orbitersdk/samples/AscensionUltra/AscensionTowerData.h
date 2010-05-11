#pragma once
#include <map>
#include "AscensionUltra.h"

class AscensionTowerData
{
public:
	AscensionTowerData(void);
	~AscensionTowerData(void);
	AscensionUltra *GetAscension();
	int GetPage();
	void SetPage(int page);
private:
	int ascensionIndex;
	char *ascensionName;
	AscensionUltra *ascension;
	std::map<UINT, char *> selectionList;
	int page;
};
