// ==============================================================
//                ORBITER MODULE: ASCENSIONTOWER
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// AscensionTower.h
// Class interface of Ascension tower MFD.
// ==============================================================

#ifndef __ASCENSIONTOWER_H
#define __ASCENSIONTOWER_H

#define WRITEMFD_HALFLINES		0x01
#define WRITEMFD_RIGHTALINED	0x04
#define WRITEMFD_HIGHLIGHTED	0x08

#include <map>
#include "AscensionTowerData.h"

class AscensionTower: public MFD {
public:
	AscensionTower (UINT mfd, DWORD w, DWORD h, VESSEL *vessel);
	~AscensionTower ();
	char *ButtonLabel (int bt);
	int ButtonMenu (const MFDBUTTONMENU **menu) const;
	void Update (HDC hDC);
	bool AscensionTower::ConsumeKeyBuffered(DWORD key);
	bool AscensionTower::ConsumeButton(int bt, int event);
	static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);
private:
	AscensionTowerData *data;
	AscensionUltra *ascension;
	float width, height;
	int mfdWidth, mfdHeight;
	HDC hDC;
	void WriteMFD(char *text, int line=-1, int column=-1, int flags=0);
	bool SelectionConsumeKeyBuffered(DWORD key);
	bool SelectionConsumeButton(int bt);
	void RenderSelectionPage();
	void RenderDoorControlPage();
	void RenderCraneControlPage();
	void RenderCraneListPage();
	void RenderPersonPage();
};

std::map<VESSEL *, std::map<UINT, AscensionTowerData *> *> g_MFDData;
HBRUSH g_Bar;
COLORREF g_MiddleGreen;

#endif // !__ASCENSIONTOWER_H