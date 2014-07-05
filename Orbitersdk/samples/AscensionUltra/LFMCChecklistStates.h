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
namespace LaunchTunnel
{
	typedef enum ListType
	{
		None=-1,
		Request,
		Preflight,
		Boarding,
		Fueling,
		Launch,
	};

	typedef enum Event
	{
		Aborted,
		Left,
		Step,
	};

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
		};
	};
}