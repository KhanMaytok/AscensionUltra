#pragma once
#include "orbitersdk.h"
#include <map>
#include "BeaconPath.h"

struct Route
{
	BeaconPath *Path;
	bool Inversed;
	bool On;
};

class Routes
{
public:
	~Routes(void);
	void Add(BeaconPath *beaconPath, const char *start, const char *end, bool inversed);
	void Clear();

	void Init(double size, double fallOff, double period, double duration, double propagate);

	// Turn all routes on and off
	virtual void Switch(bool on);

	// Check if routes are turned on
	virtual bool On();

	// Switch strobing of specified route on and off
	virtual void Switch(const char *start, const char *end, bool on);

	// Check if specified route is strobing
	virtual bool On(const char *start, const char *end);

	// Check if one of the routes from/to the given start/end point is strobing
	virtual bool AnyOn(const char *point, bool isEnd=false);

	/* Return count of all:
	   no parameters        .. available start points
	   true                 .. available end points
	   false, name of point .. end points of routes with the given start point
	   true, name of point  .. start points of routes with the given end point */
	virtual int GetPoints(bool isEnd=false, char *fromPoint=NULL);

	/* Return name of taxi-route point with given index in the list of all:
	   no additional params .. available start points
	   true                 .. available end points
	   false, name of point .. end points of routes with the given start point
	   true, name of point  .. start points of routes with the given end point */
	virtual char *GetPoint(int index, bool isEnd=false, char *fromPoint=NULL);
	
private:	
	std::map<const char *, std::map<const char *, Route *>> links;
	std::map<const char *, std::map<const char *, Route *>> reverse;
	std::map<const char *, Route *> *GetEnds(char const*start);
	std::map<const char *, Route *> *GetStarts(char const*end);
	Route *Routes::GetLink(char const*start, char const*end);
	void Routes::Switch(Route *link, bool on);
	Route *lastLink;
	bool on;
	double size, fallOff, period, duration, propagate;
};
