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

#include <map>

struct MFDData
{
	int page;
};

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
	MFDData *data;
	int width, height;
};

std::map<OBJHANDLE, std::map<UINT, MFDData *> *> g_MFDData;
HBRUSH g_Bar;
COLORREF g_MiddleGreen;

#endif // !__ASCENSIONTOWER_H