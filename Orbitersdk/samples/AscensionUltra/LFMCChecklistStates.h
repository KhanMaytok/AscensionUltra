// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// LFMCChecklistStates.h
// Definition of checklist states for preparation and launch of
// launch tunnel facility.
// ==============================================================

#pragma once
namespace LaunchTunnel
{
	namespace PrepareChecklist
	{
		typedef enum State
		{
			AbortOpen,
			Empty,
			OpenEntry,
			Entry,
			CloseEntry,
			Occupied,
			Ready,
		};

		typedef enum Event
		{
			Abort,
			Proceed,
			Revert,
		};
	}

	namespace LaunchChecklist
	{
		typedef enum State
		{
			AbortOpen,
			Empty,
			OpenExit,
			Exit,
			CloseExit,
			DeployShield,
			Launch,
			Beacons,
			Speeding,
			TakeOff,	
		};

		typedef enum Event
		{
			Abort,
		};
	}
}