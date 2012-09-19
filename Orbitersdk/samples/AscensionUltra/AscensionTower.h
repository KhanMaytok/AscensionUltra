// ==============================================================
//                ORBITER MODULE: ASCENSIONTOWER
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// AscensionTower.h
// Class interface of Ascension tower MFD.
// ==============================================================

#pragma once
#include "AscensionTowerData.h"
#define WRITEMFD_HALFLINES		0x01
#define WRITEMFD_RIGHTALINED	0x04
#define WRITEMFD_HIGHLIGHTED	0x08

#define HIGHLIGHTED(flags)	(flags & WRITEMFD_HIGHLIGHTED)>0
#define HALFLINES(flags)	(flags & WRITEMFD_HALFLINES)>0
#define RIGHTALINED(flags)	(flags & WRITEMFD_RIGHTALINED)>0

class AscensionTowerData;

class AscensionTower: public MFD {
public:
	AscensionTower (UINT mfd, DWORD w, DWORD h, VESSEL *vessel);
	char *ButtonLabel (int bt);
	int ButtonMenu (const MFDBUTTONMENU **menu) const;
	void Update (HDC hDC);
	bool AscensionTower::ConsumeKeyBuffered(DWORD key);
	bool AscensionTower::ConsumeButton(int bt, int event);
	static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);
	void WriteMFD(char *text, int line, int column, int flags);
	
private:
	AscensionTowerData *data;
	HDC hDC;
};