// Copyright (C) 2010-2014 Friedrich Kastner-Masilko
//
// This file is part of the Ascension Ultra Project
//
// Ascension Ultra is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of
// the License, or (at your option) any later version.
//
// Ascension Ultra is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Ascension Ultra. If not, see
// <http://www.gnu.org/licenses/>.

#pragma once
#include <vector>
#include "..\AscensionUltra.h"
#include "..\AscensionTower.h"
#include "AscensionTowerPage.h"
#include "orbitersdk.h"

class AscensionTowerPage;

class AscensionTowerData
{
public:
	AscensionTowerData();
	~AscensionTowerData();
	void Update();
	char *GetButtonLabel (int bt);
	int GetButtonMenu (MFDBUTTONMENU *mnu);
	bool SetButton(int bt);
	bool SetKey(DWORD key);
	AscensionTower *GetMFD();
	void SetMFD(AscensionTower *mfd);
	AscensionUltra *GetAscension();
	AscensionTowerPage *GetPage(enum AscensionTowerPageInstance page);
	bool StateChange(AscensionTowerPageInstance newstate);
	
private:
	AscensionTower *mfd;
	enum AscensionTowerPageInstance state;
	std::map<AscensionTowerPageInstance, AscensionTowerPage *> pages;
	void PreCheck();
};
