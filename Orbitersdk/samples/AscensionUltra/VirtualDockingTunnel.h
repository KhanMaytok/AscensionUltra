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

// ==============================================================
// Global variables

// Initializes the virtual docking hook.
// Returns 0 if hooked
//         1 if already hooked
//        -1 if already initialized by handle
//        -2 if already hooked by some other system
int vdtInit(VESSEL *handle);

// Removes the virtual docking hook.
// Returns 0 if unhooked
//         1 if still hooked, but handles unregistered
//        -1 if handle already unregistered
//        -2 if hook already released by some other system
int vdtExit(VESSEL *handle);

// Sets the docking state of the given vessel handle to the specified object.
// If the object is NULL, a previous dock state is cleared.
// Returns 0 if successful
//        -1 if clearing was not necessary
int vdtSetDockState(VESSEL *handle, OBJHANDLE obj);

float vdtGetVersion();