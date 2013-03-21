/*
 * ModelChecker.cpp
 *
 *  Created on: Mar 1, 2013
 *      Author: hamed
 */

#include "ModelChecker.h"
using namespace std;

IntervalSet* ModelChecker::visitStocRegion(Region* region, Formula* psi1, Formula* psi2, IntervalSet* potentialSatSet, double t, Interval bound) {
	//TODO:[IMPORTANT assumption] t=0 and bound = [0, MAX_TIME]

	if (region == NULL) return NULL;
	//if line t is within the region


	//if line t is NOT within the region
	Polygon *psi1_poly = geometryHelper->createPolygon(region, psi1);
	Polygon *psi2_poly = geometryHelper->createPolygon(region, psi2);
	psi1_poly = geometryHelper->cropPolygon(psi1_poly, bound.end, DOWN);
	psi2_poly = geometryHelper->cropPolygon(psi2_poly, bound.end, DOWN);

	if (psi1_poly != NULL) psi1_poly->print();
	if (psi2_poly != NULL) psi2_poly->print();

//	cout << "PotentialSatSet: ";
//	potentialSatSet->print();
//	psi1_poly->print();
//	std::cout<< "------------------------------" << std::endl;
//	psi2_poly->print();

	if (!debugImage.empty()){
		if (psi1_poly != NULL) geometryHelper->drawPolygon(debugImage, psi1_poly, scale, cv::Scalar(0, 0, 255));
		if (psi2_poly != NULL) geometryHelper->drawPolygon(debugImage, psi2_poly, scale, cv::Scalar(255, 255, 0));
		cv::Mat flipped;
		cv::flip(debugImage, flipped, 0);
		cv::imshow("test", flipped);
		cv::waitKey(0);
	}


	//potentialSatSet->print();

	// First interval set.
	if (psi2_poly != NULL){
		for (unsigned int i = 0; i < potentialSatSet->intervals.size(); i++){
			Point p1(potentialSatSet->intervals.at(i).start, region->lowerBoundry->getY(potentialSatSet->intervals.at(i).start));
			Point p2(potentialSatSet->intervals.at(i).end, region->lowerBoundry->getY(potentialSatSet->intervals.at(i).end));
			Segment* seg = new Segment(p1, p2);

			satSet = satSet->unionWith(geometryHelper->getIntersectionIntervals(psi2_poly, seg));
		}
	}
//	satSet->print();

	if (psi1_poly != NULL){
		IntervalSet* psi1_Int = new IntervalSet();
		//Second interval set.
		for (unsigned int i = 0; i < potentialSatSet->intervals.size(); i++){
			Point p1(potentialSatSet->intervals.at(i).start, region->lowerBoundry->getY(potentialSatSet->intervals.at(i).start));
			Point p2(potentialSatSet->intervals.at(i).end, region->lowerBoundry->getY(potentialSatSet->intervals.at(i).end));
			Segment* seg = new Segment(p1, p2);

			psi1_Int = psi1_Int->unionWith(geometryHelper->getIntersectionIntervals(psi1_poly, seg));
			cout << "psi1_Int: ";
//			psi1_Int->print();
			//In general case here the psi1_poly should be reformed.
		}

		if (psi2_poly != NULL){
			IntervalSet* psi12_Int = geometryHelper->getIntersectionIntervals(psi1_poly, psi2_poly);
			if (psi12_Int != NULL){
//				psi12_Int->print();
				satSet = satSet->unionWith(psi1_Int->intersect(psi12_Int));
//				satSet->print();
			}
		}

		//Third Set of intervals.
		for (unsigned int i = 0; i < region->successors->size(); i++){
			IntervalSet* potPsi1_int = geometryHelper->getIntersectionIntervals(psi1_poly, region->successors->at(i)->lowerBoundry);
			if (potPsi1_int != NULL){
				potPsi1_int = potPsi1_int->minus(satSet);
				potPsi1_int = potPsi1_int->intersect(psi1_Int);
				if (!potPsi1_int->isEmpty())
					 satSet = satSet->unionWith(visitStocRegion(region->successors->at(i), psi1, psi2, potPsi1_int, t, bound));
			}
		}
	}

	return satSet;

}

ModelChecker::~ModelChecker() {
	// TODO Auto-generated destructor stub
}

IntervalSet* ModelChecker::visitDtrmRegion(DtrmEvent* dtrmRegion, Formula* psi1, Formula* psi2, double t, Interval bound) {
//	satSet->print();

	if (dtrmRegion->time >= bound.end || dtrmRegion->time >= model->MaxTime || dtrmRegion == NULL)
		return satSet;

	Direction d1 = UP;
	Direction d2 = UP;
	double t1 = INF;
	double t2 = INF;

	RegionState state1 = geometryHelper->getTimeAndDirection(dtrmRegion, psi1, t1, d1);
	RegionState state2 = geometryHelper->getTimeAndDirection(dtrmRegion, psi2, t2, d2);

	if (!debugImage.empty()){
		geometryHelper->drawVerticalLine(debugImage, t1, d1, scale, cv::Scalar(0, 0, 255));
		geometryHelper->drawVerticalLine(debugImage, t2, d2, scale, cv::Scalar(255, 255, 0));
		cv::Mat flipped;
		cv::flip(debugImage, flipped, 0);
		cv::imshow("test", flipped);
		cv::waitKey(0);

	}


	if (state2 != NONE && d2 == DOWN){
		satSet = satSet->unionWith(Interval(dtrmRegion->time, model->MaxTime));
		return satSet;
	}

	if (state1 != ENTIRE && d1 == UP)
		return NULL;

	if (state2 != NONE && t2 <= t1)
		satSet = satSet->unionWith(Interval(t2, model->MaxTime));
	else if(state1 == ENTIRE && (state2 != NONE && d2 == UP))
		satSet = satSet->unionWith(Interval(t2, model->MaxTime));
	else if(state1 == ENTIRE)
		satSet = satSet->unionWith(visitDtrmRegion(dtrmRegion->nextDtrmEvent, psi1, psi2, t, bound));

//	satSet->print();

	IntervalSet* iSet = new IntervalSet();
	iSet->intervals.push_back(Interval(dtrmRegion->time, (state1 == ENTIRE)? dtrmRegion->nextDtrmEvent->time : t1));
	iSet = iSet->minus(satSet);

//	iSet->print();


	for (unsigned int i =0; i < dtrmRegion->nextRegions->size(); i++)
		satSet = satSet->unionWith(visitStocRegion(dtrmRegion->nextRegions->at(i), psi1, psi2, iSet, t, bound));

	return satSet;

}

IntervalSet* ModelChecker::until(Formula* psi1, Formula* psi2, Interval bound, double time) {
	// TODO: Currently I have assumed that the given bound starts at 0 ie. T_1 = 0.

//	if (satSet != NULL)
//		satSet->clear();
//	else
	satSet = new IntervalSet();

	double t = time; // - std->getTrEnabledTime();
	std::cout << "t" << t << std::endl;
	Line timeLine(0, t);
	Point p1, p2;

	if (!debugImage.empty())
		geometryHelper->drawVerticalLine(debugImage, bound.end, UP, scale, cv::Scalar(0, 0, 0));


//	satSet->print();
	//iterating over all deterministic events.
	// the last event regards the maximum time reached. so should not be considered.
	for (unsigned int i = 0; i < std->dtrmEventList.size() - 1; i++){
		std::cout << i << std::endl;
		if (std->dtrmEventList[i]->time <= time && std->dtrmEventList[i]->nextDtrmEvent->time > time){
			satSet = satSet->unionWith(visitDtrmRegion(std->dtrmEventList[i], psi1, psi2,t, bound));
//			satSet->print();
			break;
		}
	}

	//iterating over all stochastic regions.
	for (unsigned int i = 0; i < std->regionList.size(); i++){
		if (std->regionList[i]->intersect(timeLine, p1, p2)){
			IntervalSet* potSet = new IntervalSet();
			potSet->intervals.push_back(Interval(p1.X, p2.X));

			satSet = satSet->unionWith(visitStocRegion(std->regionList[i], psi1, psi2, potSet, t, bound));
		}
	}



	return satSet;
}

double ModelChecker::calcProb(IntervalSet* iSet, double(*cdf)(double), double shift){
	double prob = 0;
	for (unsigned int i = 0; i < iSet->intervals.size(); i++)
		prob += ( cdf(iSet->intervals[i].end - shift) - cdf(iSet->intervals[i].start - shift));
	return prob;

}

