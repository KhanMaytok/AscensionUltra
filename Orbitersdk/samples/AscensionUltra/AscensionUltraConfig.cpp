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
#include "Conf_resource.h"
#include "AscensionUltraConfigurator.h"
#include "AscensionUltraConfig.h"
#include "AscensionUltraSpawner.h"

extern gParamsType gParams;

BOOL CALLBACK AscensionUltraConfig::WndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG:
		{
			SendMessage(GetDlgItem(hWnd,IDC_CHKSPAWN), BM_SETCHECK,(WPARAM) gParams.Spawn?BST_CHECKED:BST_UNCHECKED,(LPARAM) 0);
			SendMessage(GetDlgItem(hWnd,IDC_CHKRESET), BM_SETCHECK,(WPARAM) gParams.Reset?BST_CHECKED:BST_UNCHECKED,(LPARAM) 0);
			SendMessage(GetDlgItem(hWnd,IDC_CHKSCNSAVE), BM_SETCHECK,(WPARAM) gParams.SCNSave?BST_CHECKED:BST_UNCHECKED,(LPARAM) 0);
			SendMessage(GetDlgItem(hWnd,IDC_CHKRECSAVE), BM_SETCHECK,(WPARAM) gParams.RecSave?BST_CHECKED:BST_UNCHECKED,(LPARAM) 0);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD (wParam)) {
		case IDOK:
			{
				gParams.Spawn=SendMessage(GetDlgItem(hWnd,IDC_CHKSPAWN), BM_GETCHECK,(WPARAM) 0,(LPARAM) 0)==BST_CHECKED;
				gParams.Reset=SendMessage(GetDlgItem(hWnd,IDC_CHKRESET), BM_GETCHECK,(WPARAM) 0,(LPARAM) 0)==BST_CHECKED;
				gParams.SCNSave=SendMessage(GetDlgItem(hWnd,IDC_CHKSCNSAVE), BM_GETCHECK,(WPARAM) 0,(LPARAM) 0)==BST_CHECKED;
				gParams.RecSave=SendMessage(GetDlgItem(hWnd,IDC_CHKRECSAVE), BM_GETCHECK,(WPARAM) 0,(LPARAM) 0)==BST_CHECKED;
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
bool AscensionUltraConfig::clbkOpen (HWND hLaunchpad){return OpenDialog (gParams.hInst, hLaunchpad, IDD_DGCONFIG, WndProc);}