// ==============================================================
//             ORBITER LIBRARY: VIRTUALDOCKINGTUNNEL
//                  
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// VirtualDockingTunnel.h
// Static implementation of late-binding virtual docking
// functions.
// ==============================================================
#include "orbitersdk.h"

#define ORBITEREXTENSIONSHOOK_GETDOCKSTATUS	0
#define ORBITEREXTENSIONSHOOK_RECORDEVENT	1

namespace OrbiterExtensions
{

// Initializes the virtual docking hook.
// Returns 0 if hooked
//         1 if already hooked
//        -1 if already initialized by handle
//        -2 if already hooked by some other system
//        -3 if specified hook is unknown
//		  -4 if library not found
//		  -5 internal error
int Init(VESSEL *handle, int hook=ORBITEREXTENSIONSHOOK_GETDOCKSTATUS);

// Removes the virtual docking hook.
// Returns 0 if unhooked
//         1 if still hooked, but handles unregistered
//        -1 if handle already unregistered
//        -2 if hook already released by some other system
//        -3 if specified hook is unknown
//		  -4 if library not found
int Exit(VESSEL *handle, int hook=ORBITEREXTENSIONSHOOK_GETDOCKSTATUS);

// Sets the docking state of the given vessel handle to the specified object.
// If the object is NULL, a previous dock state is cleared.
// Returns 0 if successful
//        -1 if clearing was not necessary
//		  -4 if library not found
int SetDockState(VESSEL *handle, OBJHANDLE obj);

// Returns version of Orbiter extensions DLL or -4 if library was not found
float GetVersion();

//*** Version 0.2 functions

// Register the given vessel for RecordEvent hooking and/or sets the specified backlog.
// Returns 0 if successful
//		  -4 if library not found
int RecordVesselEvents(VESSEL *handle, int backlog);

// Unregister the given vessel for RecordEvent hooking and delete the queue.
// Returns 0 if successful
//        -1 if vessel was not registered
//		  -4 if library not found
int DeleteVesselEvents(VESSEL *handle);

// Gets the specified amount of recorded events for the given vessel.
// The arrays needs to be initialized with the appropriate count.
// Returns 0 if queue was empty
//        >0 number of read events
//        -1 if vessel was not registered
//		  -4 if library not found
int GetVesselEvents(VESSEL *handle, double *mjds, char **event_types, char **events, int size);

// Cleans the specified recorded events.
// Returns 0 if successfull
//		  -4 if library not found
int CleanVesselEvents(char **event_types, char **events, int size);

}