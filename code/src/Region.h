/*
 * Region.h
 *
 *  Created on: 28 okt. 2011
 *      Author: GhasemiehH
 */

#ifndef REGION_H_
#define REGION_H_

#include <vector>
#include <iostream>

#include "Line.h"
#include "Event.h"

class Region {
public:
	Region(std::vector<StochasticEvent*> * eventList, Segment * lowerBoundry);


	std::vector<StochasticEvent*>* eventSegments; //list of surrounding segments.
	Segment* lowerBoundry;
	Segment* leftBoundry;
	Segment* rightBoundry;

	/**
	 * Each region should have a time bias. which means this is belong to which segment of main stochastic and determinestic line. 
	 * 
	 */
	double timeBias;

	/**
	 * This marking indicates the marking exactly after the happening of the event which caused us entering this region.
	 * so in order to finding of places marking an advancement according to the specified time is needed.
	 */
	Marking* marking;

	void print(std::ostream &out);

	bool intersect(Segment& s, Point &p1, Point &p2);
};

#endif /* REGION_H_ */
