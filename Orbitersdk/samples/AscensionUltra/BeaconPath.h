#pragma once
#include "orbitersdk.h"
#include <list>
#include "BeaconArray.h"

class BeaconPath
{
public:
	~BeaconPath(void);
	void Init(VESSEL *owner, VECTOR3 *arrays, VECTOR3 &color, int *beacons, int length);
	void Add(BeaconArray *beaconArray);
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

private:	
	void CalculateStrobe(double period, double duration, double propagate);
	std::list <BeaconArray *> arrays;
	double offset;
};
