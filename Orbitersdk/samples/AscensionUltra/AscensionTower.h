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

class AscensionTower: public MFD {
public:
	AscensionTower (DWORD w, DWORD h, VESSEL *vessel);
	~AscensionTower ();
	char *ButtonLabel (int bt);
	int ButtonMenu (const MFDBUTTONMENU **menu) const;
	void Update (HDC hDC);
	static int MsgProc (UINT msg, UINT mfd, WPARAM wparam, LPARAM lparam);
};

#endif // !__ASCENSIONTOWER_H