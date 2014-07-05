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
#include "GroupElement.h"

class BeaconArray : public GroupElement
{
public:
	BeaconArray(void);
	~BeaconArray(void);
	void Init(VESSEL *owner, VECTOR3 &start, VECTOR3 &end, VECTOR3 &col, int beacons);
	void SetLine(VECTOR3 &start, VECTOR3 &end);
	void SetColor(VECTOR3 &color);
	void SetBeacons(int beacons);
	void SetShape(DWORD shape);
	void SetSize(double size);
	void SetFallOff(double falloff);
	void GetLine(VECTOR3 &start, VECTOR3 &end);
	void GetColor(VECTOR3 &color);
	int GetBeacons();
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
	double GetOffsetPropagation();

	virtual GroupElementType GetType() {return TypeBeaconArray;}
	virtual void Displace(VECTOR3 offset) {SetPositions(start+offset, end+offset);}

private:
	void Clear();
	void CalculateStrobe();
	void SetPositions(VECTOR3 &start, VECTOR3 &end);
	VESSEL *owner;
	BEACONLIGHTSPEC *spec;	
	int beacons;
	VECTOR3 start, end, color;
	double period, duration, propagate, offset, offsetPropagation;
};
