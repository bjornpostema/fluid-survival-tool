/*
 * Formula.h
 *
 *  Created on: Mar 1, 2013
 *      Author: hamed
 */

#ifndef FORMULA_H_
#define FORMULA_H_
#include "IntervalSet.h"
#include "DFPN2.h"
#include <string.h>

namespace model {

enum Formula_type {FORMULA, DISCRETE, CONTINUOUS, COMBI, AND, NEG, UNTIL, TT, PROB};
enum Operator {LEQ, LESSER, GREATER, GEQ};

class Formula {
private:
    Formula *leftChild;
    Formula *rightChild;
public:
    virtual Formula_type getType(){return FORMULA;};
    Formula(Formula *_leftChild, Formula *_rightChild) : leftChild(_leftChild), rightChild(_rightChild) {};
    virtual ~Formula(){};
    virtual Formula* getLeftChild() {
        return leftChild;
    }

    virtual Formula* getRightChild() {
        return rightChild;
    }
};

class ProbFormula : public Formula {
private:
    double prob; // probability to compare with
    Operator op;
public:
    ProbFormula(Formula *leftChild, Formula *rightChild, double _prob, Operator _op): Formula(leftChild,rightChild), prob(_prob), op(_op){};
    Formula_type getType() {return PROB;};

    double getProb() {
        return prob;
    }

    Operator getOp() {
        return op;
    }
};


class AtomDisFormula : public Formula {

private:
    int placeIndex;
    char *placeName;
    int n;

public:
    AtomDisFormula(Formula *leftChild, Formula *rightChild, char *_placeName, int _n): Formula(leftChild,rightChild), placeName(_placeName), n(_n){};
    Formula_type getType() {return DISCRETE;};
	int getN() const {
		return n;
	}

    char *getPlaceName() {
        return placeName;
	}

    bool setPlaceIndex(Model *m) {
        int pIndex = -1;
        for (int i = 0; pIndex == -1 && i < m->N_places; i++) {
            if (m->places[i].type != PT_DISCRETE) continue;
            if (strncmp(m->places[i].id,placeName, strlen(m->places[i].id)) != 0) continue;
            pIndex = i;
        }

        placeIndex = pIndex;
        return (pIndex != -1);
    }

    unsigned int getPlaceIndex() {
        return placeIndex;
    }
};

class AtomContFormula : public Formula {
private:
    int placeIndex;
    char *placeName;
	double c;

public:
	/**
	 * @param placeIndex index of place \f$ x \f$.
	 * @param c camparison value.
	 *
	 * \brief Contains atomic formula of type \f$x \geq c \f$.
	 */
    AtomContFormula(Formula *leftChild, Formula *rightChild, char *_placeName, double _c): Formula(leftChild,rightChild), placeName(_placeName), c(_c){};

	Formula_type getType() {return CONTINUOUS;};

    double getC() {
		return c;
	}

    char *getPlaceName() {
        return placeName;
	}

    bool setPlaceIndex(Model *m) {
        int pIndex = -1;
        for (int i = 0; pIndex == -1 && i < m->N_places; i++) {
            if (m->places[i].type != PT_FLUID) continue;
            if (strncmp(m->places[i].id,placeName, strlen(m->places[i].id)) != 0) continue;
            pIndex = i;
        }

        placeIndex = pIndex;
        return (pIndex != -1);
    }

    unsigned int getPlaceIndex() {
        return placeIndex;
    }
};

class TrueFormula : public Formula {

private:

public:
    TrueFormula(Formula *leftChild, Formula *rightChild) : Formula(leftChild,rightChild) {};
    Formula_type getType() {return TT;};
};

class NegFormula : public Formula {

private:

public:
    NegFormula(Formula *leftChild, Formula *rightChild) : Formula(leftChild,rightChild) {};
    Formula_type getType() {return NEG;};
};

class AndFormula : public Formula {

private:

public:
    AndFormula(Formula *leftChild, Formula *rightChild) : Formula(leftChild,rightChild) {};
    Formula_type getType() {return AND;};
};

class UntilFormula : public Formula {

private:
Interval bound;

public:
    UntilFormula(Formula *leftChild, Formula *rightChild, Interval _bound) : Formula(leftChild,rightChild),bound(_bound) {};
    Formula_type getType() {return UNTIL;};

    Interval getBound() {
        return bound;
    }
};

///*
// * Temporary class! because of deadline!
// */
class ADFormula: public Formula{
public:
    ADFormula(Formula *leftChild, Formula *rightChild, AtomContFormula* cont, AtomDisFormula* dis): Formula(leftChild,rightChild), contFormula(cont), disFormula(dis){};

    Formula_type getType() {return COMBI;};
    AtomContFormula* contFormula;
    AtomDisFormula* disFormula;

};

}

#endif /* FORMULA_H_ */
