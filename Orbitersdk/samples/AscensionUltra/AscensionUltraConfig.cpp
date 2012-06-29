// ==============================================================
//        ORBITER STARTUP MODULE: ASCENSIONULTRACONFIGURATOR
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// AscensionUltraConfigurator.cpp
// Implementation of Ascension Ultra startup module.
// Contains launchpad items and autoloading tools.
// ==============================================================

#pragma once
#include "Conf_resource.h"
#include "AscensionUltraConfigurator.h"
#include "AscensionUltraConfig.h"

extern gParamsType gParams;

BOOL CALLBACK AscensionUltraConfig::WndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG:
		{
			bool state=GetPrivateProfileInt(SECTION, SPAWN, 1, INIFILE)!=0;
			SendMessage(GetDlgItem(hWnd,IDC_CHKSPAWN), BM_SETCHECK,(WPARAM) state?BST_CHECKED:BST_UNCHECKED,(LPARAM) 0);
			state=GetPrivateProfileInt(SECTION, RESET, 1, INIFILE)!=0;
			SendMessage(GetDlgItem(hWnd,IDC_CHKRESET), BM_SETCHECK,(WPARAM) state?BST_CHECKED:BST_UNCHECKED,(LPARAM) 0);
			state=GetPrivateProfileInt(SECTION, SCNSAVE, 1, INIFILE)!=0;
			SendMessage(GetDlgItem(hWnd,IDC_CHKSCNSAVE), BM_SETCHECK,(WPARAM) state?BST_CHECKED:BST_UNCHECKED,(LPARAM) 0);
			state=GetPrivateProfileInt(SECTION, RECSAVE, 1, INIFILE)!=0;
			SendMessage(GetDlgItem(hWnd,IDC_CHKRECSAVE), BM_SETCHECK,(WPARAM) state?BST_CHECKED:BST_UNCHECKED,(LPARAM) 0);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD (wParam)) {
		case IDOK:
			{
				bool state=SendMessage(GetDlgItem(hWnd,IDC_CHKSPAWN), BM_GETCHECK,(WPARAM) 0,(LPARAM) 0)==BST_CHECKED;
				WritePrivateProfileString(SECTION, SPAWN, state?"1":"0", INIFILE);
				state=SendMessage(GetDlgItem(hWnd,IDC_CHKRESET), BM_GETCHECK,(WPARAM) 0,(LPARAM) 0)==BST_CHECKED;
				WritePrivateProfileString(SECTION, RESET, state?"1":"0", INIFILE);
				state=SendMessage(GetDlgItem(hWnd,IDC_CHKSCNSAVE), BM_GETCHECK,(WPARAM) 0,(LPARAM) 0)==BST_CHECKED;
				WritePrivateProfileString(SECTION, SCNSAVE, state?"1":"0", INIFILE);
				state=SendMessage(GetDlgItem(hWnd,IDC_CHKRECSAVE), BM_GETCHECK,(WPARAM) 0,(LPARAM) 0)==BST_CHECKED;
				WritePrivateProfileString(SECTION, RECSAVE, state?"1":"0", INIFILE);
			}
			EndDialog (hWnd, 0);
			return 0;
		case IDCANCEL:
			EndDialog (hWnd, 0);
			return 0;
		}
		break;
	}
	return 0;
}

char *AscensionUltraConfig::Name() { return "Ascension Configuration"; }
char *AscensionUltraConfig::Description() { return "Global configuration for Ascension Island."; }
bool AscensionUltraConfig::clbkOpen (HWND hLaunchpad) { return OpenDialog (gParams.hInst, hLaunchpad, IDD_DGCONFIG, WndProc); }	