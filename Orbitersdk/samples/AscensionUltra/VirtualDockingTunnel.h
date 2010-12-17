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

namespace OrbiterExtensions
{

// Initializes the virtual docking hook.
// Returns 0 if hooked
//         1 if already hooked
//        -1 if already initialized by handle
//        -2 if already hooked by some other system
//		  -3 if library not found
//		  -4 internal error
int Init(VESSEL *handle);

// Removes the virtual docking hook.
// Returns 0 if unhooked
//         1 if still hooked, but handles unregistered
//        -1 if handle already unregistered
//        -2 if hook already released by some other system
//		  -3 if library not found
int Exit(VESSEL *handle);

// Sets the docking state of the given vessel handle to the specified object.
// If the object is NULL, a previous dock state is cleared.
// Returns 0 if successful
//        -1 if clearing was not necessary
//		  -3 if library not found
int SetDockState(VESSEL *handle, OBJHANDLE obj);

// Returns version of Orbiter extensions DLL or -3 if library was not found
float GetVersion();

}