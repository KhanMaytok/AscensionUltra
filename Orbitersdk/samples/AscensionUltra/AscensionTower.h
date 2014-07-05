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
#include "Pages\AscensionTowerData.h"
#define WRITEMFD_HALFLINES		0x01
#define WRITEMFD_RIGHTALIGNED	0x04
#define WRITEMFD_HIGHLIGHTED	0x08

#define HIGHLIGHTED(flags)	(flags & WRITEMFD_HIGHLIGHTED)>0
#define HALFLINES(flags)	(flags & WRITEMFD_HALFLINES)>0
#define RIGHTALIGNED(flags)	(flags & WRITEMFD_RIGHTALIGNED)>0

class AscensionTowerData;

class AscensionTower: public MFD2 {
public:
	AscensionTower (UINT mfd, DWORD w, DWORD h, VESSEL *vessel);
	char *ButtonLabel (int bt);
	int ButtonMenu (const MFDBUTTONMENU **menu) const;
	bool Update (oapi::Sketchpad *pad);
	bool AscensionTower::ConsumeKeyBuffered(DWORD key);
	bool AscensionTower::ConsumeButton(int bt, int event);
	static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);
	void Write(char *text, int line=-1, int column=-1, int flags=0);
	void SetWriteStyle(int font=0, int color=0, int intensity=0, int style=1);
	VESSEL *GetVessel();
	
private:
	AscensionTowerData *data;
	oapi::Sketchpad *pad;
	VESSEL *vessel;
};