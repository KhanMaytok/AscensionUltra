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

HMODULE g_Module;
VESSELFUNC g_Init;
VESSELFUNC g_Exit;
MAPFUNC g_SetDockState;
FLOATGETTER g_GetVersion;

// Initializes the virtual docking hook.
// Returns 0 if hooked
//         1 if already hooked
//        -1 if already initialized by handle
//        -2 if already hooked by some other system
//		  -3 if library not found
//		  -4 internal error
int vdtInit(VESSEL *handle)
{
	HMODULE g_Module=LoadLibrary("Modules\\VirtualDockingTunnel.dll");
	if (g_Module==NULL) return -3;
	g_Init=(VESSELFUNC)GetProcAddress(g_Module, "Init");
	g_Exit=(VESSELFUNC)GetProcAddress(g_Module, "Exit");
	g_SetDockState=(MAPFUNC)GetProcAddress(g_Module, "SetDockState");
	g_GetVersion=(FLOATGETTER)GetProcAddress(g_Module, "GetVersion");
	if (((DWORD)g_Init | (DWORD)g_Exit | (DWORD)g_SetDockState | (DWORD)g_GetVersion)==NULL) return -4;
	return g_Init(handle);
}

// Removes the virtual docking hook.
// Returns 0 if unhooked
//         1 if still hooked, but handles unregistered
//        -1 if handle already unregistered
//        -2 if hook already released by some other system
int vdtExit(VESSEL *handle)
{
	int result=g_Exit(handle);
	if (result!=1) FreeLibrary(g_Module);
	return result;
}

// Sets the docking state of the given vessel handle to the specified object.
// If the object is NULL, a previous dock state is cleared.
// Returns 0 if successful
//        -1 if clearing was not necessary
int vdtSetDockState(VESSEL *handle, OBJHANDLE obj){return g_SetDockState(handle, obj);}

float vdtGetVersion(){return g_GetVersion();}