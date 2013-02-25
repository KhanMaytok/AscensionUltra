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
	namespace RequestChecklist
	{
		typedef enum State
		{
			Empty,
			LFHold,
			Wait,
			Roll,
		};

		typedef enum Event
		{
			Abort,
		};
	};

	namespace PreflightChecklist
	{
		typedef enum State
		{
			AbortOpen,
			Empty,
			OpenEntry,
			Entry,
			PFHold,
			Wait,
			Roll,
		};

		typedef enum Event
		{
			Abort,
			Proceed,
		};
	};

	namespace BoardingChecklist
	{
		typedef enum State
		{
			AbortWait,
			Empty,
			Taxi,
			PAXHold,
			Wait,
			Roll,
		};

		typedef enum Event
		{
			Abort,
			Proceed,
		};
	};

	namespace FuelingChecklist
	{
		typedef enum State
		{
			AbortWait,
			Empty,
			Taxi,
			FuelHold,
			Wait,
			Roll,
		};

		typedef enum Event
		{
			Abort,
			Proceed,
		};
	};

	namespace LaunchChecklist
	{
		typedef enum State
		{
			AbortOpen,
			Empty,
			OpenExit,
			Exit,
			Blast,
			LaunchHold,
			Beacons,
			Speeding,
			TakeOff,	
		};

		typedef enum Event
		{
			Abort,
			Proceed,
			Aborted,
			Left,
		};
	};
}