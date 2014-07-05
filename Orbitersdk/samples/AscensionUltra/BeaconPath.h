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
#include "orbitersdk.h"
#include <list>
#include "BeaconArray.h"
#include "GroupElement.h"

class BeaconPath : public GroupElement
{
public:
	~BeaconPath(void);
	void Init(VESSEL *owner, VECTOR3 *arrays, VECTOR3 &color, int *beacons, int length);
	void Add(BeaconArray *beaconArray, bool inverse=false);
	void Clear();
	void SetColor(VECTOR3 &color);
	void SetShape(DWORD shape);
	void SetSize(double size);
	void SetFallOff(double falloff);
	void GetColor(VECTOR3 &color);
	DWORD GetShape();
	double GetSize();
	double GetFallOff();
	void Switch(bool on);
	bool On();

	/* Period of strobe animation -
	   how long it takes in seconds from animation begin until the cycle repeats
	   <0..direction is inversed
	   =0..all beacons are synchronized */
	void SetPeriod(double period);
	double GetPeriod();

	/* Duration of light emitting -
	   how long one beacon is lit in seconds
	   <0..beacon is lit always except for specified time
	   =0..beacon is always lit */
	void SetDuration(double duration); 
	double GetDuration();

	/* Propagation of light in array -
	   how long after the last beacon was lit should the next be switched on in
	   seconds
	   <0 ..Overlapping - more than one beacon lit concurrently */
	void SetPropagate(double propagate);
	double GetPropagate();

	/* Offset of strobe animation -
	   how far off is the animation's timing from the virtual zero point */
	void SetOffset(double offset);
	double GetOffset();

	virtual GroupElementType GetType() {return TypeBeaconPath;}

private:	
	struct Item
	{
		bool inversed;
		BeaconArray *beaconArray;
	};
	void CalculateStrobe(double period, double duration, double propagate);
	std::list <Item> arrays;
	std::list <BeaconArray *> selfCreated;
	double offset;	
};
