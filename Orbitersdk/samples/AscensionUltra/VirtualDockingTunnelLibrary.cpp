// ==============================================================
//          ORBITER LIBRARY: VIRTUALDOCKINGTUNNELLIBRARY
//                  
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// VirtualDockingTunnelLibrary.cpp
// Static implementation of late-binding virtual docking
// functions.
// ==============================================================

#include "VirtualDockingTunnel.h"

typedef int (* VESSELFUNC) (VESSEL *handle);
typedef int (* MAPFUNC) (VESSEL *handle, OBJHANDLE obj);
typedef float (* FLOATGETTER) (void);

// ==============================================================
// Global variables

namespace OrbiterExtensions
{
HMODULE g_Module;
VESSELFUNC g_Init;
VESSELFUNC g_Exit;
MAPFUNC g_SetDockState;
FLOATGETTER g_GetVersion;
}

// Initializes the virtual docking hook.
// Returns 0 if hooked
//         1 if already hooked
//        -1 if already initialized by handle
//        -2 if already hooked by some other system
//		  -3 if library not found
//		  -4 internal error
int OrbiterExtensions::Init(VESSEL *handle)
{
	g_Module=LoadLibrary("Modules\\VirtualDockingTunnel.dll");
	if (g_Module==NULL) return -3;
	g_Init=(VESSELFUNC)GetProcAddress(g_Module, "Init");
	g_Exit=(VESSELFUNC)GetProcAddress(g_Module, "Exit");
	g_SetDockState=(MAPFUNC)GetProcAddress(g_Module, "SetDockState");
	g_GetVersion=(FLOATGETTER)GetProcAddress(g_Module, "GetVersion");
	if (((DWORD)g_Init & (DWORD)g_Exit & (DWORD)g_SetDockState & (DWORD)g_GetVersion)==NULL)
	{
		FreeLibrary(g_Module);
		return -4;
	}
	return g_Init(handle);
}

// Removes the virtual docking hook.
// Returns 0 if unhooked
//         1 if still hooked, but handles unregistered
//        -1 if handle already unregistered
//        -2 if hook already released by some other system
//		  -3 if library not found
int OrbiterExtensions::Exit(VESSEL *handle)
{
	if (!g_Exit) return -3;
	int result=g_Exit(handle);
	FreeLibrary(g_Module);
	return result;
}

// Sets the docking state of the given vessel handle to the specified object.
// If the object is NULL, a previous dock state is cleared.
// Returns 0 if successful
//        -1 if clearing was not necessary
//		  -3 if library not found
int OrbiterExtensions::SetDockState(VESSEL *handle, OBJHANDLE obj){if (!g_SetDockState) return -3; return g_SetDockState(handle, obj);}

// Returns the version of the loaded library if successful
//		  -3 if library not found
float OrbiterExtensions::GetVersion(){if (!g_GetVersion) return -3; return g_GetVersion();}