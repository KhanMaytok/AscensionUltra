#pragma once
#include <map>
#include "AscensionUltra.h"

class AscensionTowerData
{
public:
	AscensionTowerData(void);
	~AscensionTowerData(void);
	AscensionUltra *GetAscension();
	void SetAscension(int index);
	int GetPage();
	void SetPage(int page);
private:
	int ascensionIndex;
	char *ascensionName;
	AscensionUltra *ascension;
	std::map<UINT, VESSEL *> scanList;
	int page;
	void Scan();
};
