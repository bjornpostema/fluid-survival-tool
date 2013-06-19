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
#include "DFPN2.h"
#include <math.h>

extern "C" {
#include <matheval.h>
}

#define yyconst const
#define STR_EQUAL 0
typedef struct yy_buffer_state *YY_BUFFER_STATE;

extern int yyparse();
extern YY_BUFFER_STATE yy_scan_string (yyconst char *yy_str);
extern class model::Formula *PARSER_FML;
extern int yylex_destroy (void );
extern model::Formula* parser_results();
extern void reset_lexer(void);
extern int getColError();

namespace model {

class ModelChecker {

private:
	Model* model;
    TimedDiagram* std;
    double ttc; // time to check
	IntervalSet* satSet;
    GeometryHelper* geometryHelper;
    Logger *guic;

    /*
     * Helper variables for the spdf functions
     */
    double mu;
    double lambda;
    double a,b;
    void *f;
    double sigma;
    int K;
    double dtrm;
    Distribution distr;

    IntervalSet* visitStocRegion(Region* region, Formula* psi1, Formula* psi2, IntervalSet* potentialSatSet, double t, Interval bound);
	IntervalSet* visitDtrmRegion(DtrmEvent* dtrmRegion, Formula* psi1, Formula* psi2, double t, Interval bound);

public:
	cv::Mat debugImage;
	int scale;

    ModelChecker(Model* model, TimedDiagram* std, Logger* guic) {this->model = model; this->std = std; this->guic = guic; geometryHelper = new GeometryHelper(model);};
    ModelChecker(Model* model, TimedDiagram* std, Logger* guic, double ttc) {this->model = model; this->std = std; this->guic = guic; this->ttc = ttc; geometryHelper = new GeometryHelper(model);};
	virtual ~ModelChecker();

    /*
     * Functions that will calculate the intervalsets by traversal.
     */
    bool iSetTT(IntervalSet *&res);
    bool iSetNeg(IntervalSet *&res, IntervalSet* iset1);
    bool iSetAnd(IntervalSet *&res, IntervalSet* iset1, IntervalSet* iset2);
    bool iSetAtomDis(IntervalSet *&res, AtomDisFormula* psi1);
    bool iSetAtomCont(IntervalSet *&res, AtomContFormula* psi1);

    /*
     * The limitations of the until function, due to the clipping of polygons is
     * that only one atomic formula can be compared to another atomic formula
     */
    bool until(IntervalSet *&satSet, Formula* psi1, Formula* psi2, Interval bound);

    IntervalSet* tempUntil(Formula* psi1, Formula* psi2, Interval bound, double time);

    /*
     * Helper function for the until formula
     */
    Polygon* polyNeg(Polygon* poly1);
    Polygon* polyAnd(Polygon* poly1, Polygon* poly2);
    Polygon* polyAtomDis(Formula* psi1);
    Polygon* polyAtomCont(Formula* psi1);

    /**
     * @brief calcAtomDisISetAtAtTime calculates the intersection at a specific time to check.
     * @param time The time for which probability calculation is being done.
     * @param pIndex The index number of the discrete place.
     * @param amount The amount of to compare with.
     * @return The intervalset that the given property holds for a given time to check.
     */
    IntervalSet* calcAtomDisISetAtTime(double time, int pIndex, double amount);

    /**
     * @brief calcAtomContISetAtAtTime calculates the intersection at a specific time to check.
     * @param time The time for which probability calculation is being done.
     * @param pIndex The index number of the continuous place.
     * @param amount The amount of to compare with.
     * @return The intervalset that the given property holds for a given time to check.
     */
    IntervalSet* calcAtomContISetAtTime(double time, int pIndex, double amount);

    /**
     * @brief calcProb calculates the probabilities for an intervalset
     * @param iSet The intervalset to integrate over
     * @param sPdfInt Pointer to the integral pdf of s.
     * @param fv function variables for the integral calculation.
     * @return The probabilities found by integrating over the intervalset
     */
    double calcProb(IntervalSet* iSet, double shift);

    bool compareProbs(double calculatedProb, double probInput);

    bool parseFML(Formula *&fullFML, QString rawFormula);
    bool traverseFML(bool &res, Formula *fullFML);
    bool traverseISetFML(IntervalSet *&res, Formula *fullFML);

    /*
     * Helper functions as in DFPN2, although this one is more generalised independant.
     */
    bool propertyXleqCTest(Model* model, Marking* marking, double t0, double t1, double &s1, double &s2, int pIndex, double amount);

    bool setVariables();
    void setLambda(double lambda) { this->lambda = lambda; }
    void setAB(double a, double b) { this->a = a; this->b = b; }
    void setMu(double mu) { this->mu = mu; }
    void setF(void *f) { this->f = f; }
    void setSigma(double sigma) { this->sigma = sigma; }
    void setK(int K) { this->K = K; }
    void setDtrm(double dtrm) { this->dtrm = dtrm; }
    void setDistr(Distribution distr) { this->distr = distr; }

    double getLambda() { return this->lambda; }
    double getA() { return this->a; }
    double getB() { return this->b; }
    double getMu() { return this->mu; }
    void* getF() { return this->f; }
    double getSigma() { return this->sigma; }
    int getK() { return this->K; }
    int getDtrm() { return this->dtrm; }

    Distribution getDistr() { return this->distr; }

    double scdfExp(double s);
    double scdfUni(double s);
    double scdfGen(double s);
    int scdfGammaFact(int n);
    double scdfGamma(double s);
    double scdfNormal(double s);
    double scdfFoldedNormal(double s);
    double scdfDtrm(double s);
};

}

#endif /* MODELCHECKER_H_ */
