/*
 * ModelChecker.h
 *
 *  Created on: Mar 1, 2013
 *      Author: hamed
 */

#include <vector>

#ifndef MODELCHECKER_H_
#define MODELCHECKER_H_

#include "Region.h"
#include "TimedDiagram.h"
#include "GeometryHelper.h"

namespace model {

class ModelChecker {

private:
	Model* model;
	TimedDiagram* std;
	IntervalSet* satSet;
	GeometryHelper* geometryHelper;


    IntervalSet* visitStocRegion(Region* region, Formula* psi1, Formula* psi2, IntervalSet* potentialSatSet, double t, Interval bound);
	IntervalSet* visitDtrmRegion(DtrmEvent* dtrmRegion, Formula* psi1, Formula* psi2, double t, Interval bound);

public:
	cv::Mat debugImage;
	int scale;

	ModelChecker(Model* model, TimedDiagram* std) {this->model = model; this->std = std; geometryHelper = new GeometryHelper(model);};
	virtual ~ModelChecker();

	IntervalSet* until(Formula* psi1, Formula* psi2, Interval bound, double time);

    double calcProb(IntervalSet* iSet, double(*cdf)(double, FunctionVars*), double shift, FunctionVars* fv);

};

}

#endif /* MODELCHECKER_H_ */
