// ==============================================================
//                ORBITER MODULE: ASCENSIONULTRA
//                  Part of Ascension Ultra
//          Copyright (C) 2010 Friedrich Kastner-Masilko
//                   All rights reserved
//
// BeaconArray.h
// Class definition of beacon arrays.
// ==============================================================

#pragma once
#include "orbitersdk.h"

class BeaconArray
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

private:
	void Clear();
	void CalculateStrobe();
	VESSEL *owner;
	BEACONLIGHTSPEC *spec;	
	int beacons;
	VECTOR3 color;
	double period, duration, propagate, offset, offsetPropagation;
};
