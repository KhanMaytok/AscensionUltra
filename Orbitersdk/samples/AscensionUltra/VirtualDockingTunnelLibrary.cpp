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

typedef int (* HOOKFUNC) (VESSEL *, int);
typedef int (* VESSELFUNC) (VESSEL *);
typedef int (* MAPFUNC) (VESSEL *, OBJHANDLE);
typedef float (* FLOATGETTER) (void);
typedef int (* GETEVENTFUNC) (VESSEL *, double *, char **, char **, int);
typedef int (* CLEANFUNC) (char **, char **, int);

// ==============================================================
// Global variables

namespace OrbiterExtensions
{
HMODULE g_Module;
VESSELFUNC g_Init;
VESSELFUNC g_Exit;
HOOKFUNC g_InitHook;
HOOKFUNC g_ExitHook;
MAPFUNC g_SetDockState;
FLOATGETTER g_GetVersion;
HOOKFUNC g_RecordVesselEvents;
VESSELFUNC g_DeleteVesselEvents;
GETEVENTFUNC g_GetVesselEvents;
CLEANFUNC g_CleanVesselEvents;
}

// Initializes the virtual docking hook.
// Returns 0 if hooked
//         1 if already hooked
//        -1 if already initialized by handle
//        -2 if already hooked by some other system
//        -3 if specified hook is unknown
//		  -4 if library not found
//		  -5 internal error
int OrbiterExtensions::Init(VESSEL *handle, int hook)
{
	g_Module=LoadLibrary("Modules\\VirtualDockingTunnel.dll");
	if (g_Module==NULL) return -4;	
	g_Init=(VESSELFUNC)GetProcAddress(g_Module, "Init");
	g_Exit=(VESSELFUNC)GetProcAddress(g_Module, "Exit");
	g_InitHook=(HOOKFUNC)GetProcAddress(g_Module, "InitHook");
	g_ExitHook=(HOOKFUNC)GetProcAddress(g_Module, "ExitHook");
	g_SetDockState=(MAPFUNC)GetProcAddress(g_Module, "SetDockState");
	g_GetVersion=(FLOATGETTER)GetProcAddress(g_Module, "GetVersion");
	g_RecordVesselEvents=(HOOKFUNC)GetProcAddress(g_Module, "RecordVesselEvents");
	g_DeleteVesselEvents=(VESSELFUNC)GetProcAddress(g_Module, "DeleteVesselEvents");
	g_GetVesselEvents=(GETEVENTFUNC)GetProcAddress(g_Module, "GetVesselEvents");
	g_CleanVesselEvents=(CLEANFUNC)GetProcAddress(g_Module, "CleanVesselEvents");
	float version=GetVersion();
	if (((DWORD)g_Init & (DWORD)g_Exit & (DWORD)g_SetDockState & (DWORD)g_GetVersion)==NULL ||
		(((DWORD)g_InitHook & (DWORD)g_ExitHook & (DWORD)g_RecordVesselEvents & (DWORD)g_DeleteVesselEvents & (DWORD)g_GetVesselEvents & (DWORD)g_CleanVesselEvents)==NULL && version>0.1f))
	{
		FreeLibrary(g_Module);
		g_GetVersion=(FLOATGETTER)NULL;
		g_Init=g_Exit=(VESSELFUNC)NULL;
		g_SetDockState=(MAPFUNC)NULL;
		g_InitHook=g_ExitHook=(HOOKFUNC)NULL;
		return -5;
	}
	if (version>=0.2f) return g_InitHook(handle, hook);
	if (hook!=0)
	{
		FreeLibrary(g_Module);
		return -3;
	}
	return g_Init(handle);
}

// Removes the virtual docking hook.
// Returns 0 if unhooked
//         1 if still hooked, but handles unregistered
//        -1 if handle already unregistered
//        -2 if hook already released by some other system
//        -3 if specified hook is unknown
//		  -4 if library not found
int OrbiterExtensions::Exit(VESSEL *handle, int hook)
{
	if (!g_Exit) return -4;
	float version=GetVersion();
	int result;
	if (version>=0.2f) result=g_ExitHook(handle, hook);
	else if (hook!=0) result=-3;
	else result=g_Exit(handle);
	if (result!=-3) FreeLibrary(g_Module);
	return result;
}

// Sets the docking state of the given vessel handle to the specified object.
// If the object is NULL, a previous dock state is cleared.
// Returns 0 if successful
//        -1 if clearing was not necessary
//		  -4 if library not found
int OrbiterExtensions::SetDockState(VESSEL *handle, OBJHANDLE obj){if (!g_SetDockState) return -3; return g_SetDockState(handle, obj);}

// Returns the version of the loaded library if successful
//		  -4 if library not found
float OrbiterExtensions::GetVersion(){if (!g_GetVersion) return -4; return g_GetVersion();}

//*** Version 0.2 functions

// Register the given vessel for RecordEvent hooking and/or sets the specified backlog.
// Returns 0 if successful
//		  -4 if library not found
int OrbiterExtensions::RecordVesselEvents(VESSEL *handle, int backlog){if (!g_RecordVesselEvents) return -4; return g_RecordVesselEvents(handle, backlog);}

// Unregister the given vessel for RecordEvent hooking and delete the queue.
// Returns 0 if successful
//        -1 if vessel was not registered
//		  -4 if library not found
int OrbiterExtensions::DeleteVesselEvents(VESSEL *handle){if (!g_DeleteVesselEvents) return -4; return g_DeleteVesselEvents(handle);}

// Gets the specified amount of recorded events for the given vessel.
// The arrays needs to be initialized with the appropriate count.
// Returns 0 if queue was empty
//        >0 number of read events
//        -1 if vessel was not registered
//		  -4 if library not found
int OrbiterExtensions::GetVesselEvents(VESSEL *handle, double *mjds, char **event_types, char **events, int size){if (!g_GetVesselEvents) return -4; return g_GetVesselEvents(handle, mjds, event_types, events, size);}

// Cleans the specified recorded events.
// Returns 0 if successfull
//		  -4 if library not found
int OrbiterExtensions::CleanVesselEvents(char **event_types, char **events, int size){if (!g_CleanVesselEvents) return -4; return g_CleanVesselEvents(event_types, events, size);}