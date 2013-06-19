/*
 * Formula.h
 *
 *      Author: Bjorn
 */

#ifndef FORMULA_STRUCT_H_
#define FORMULA_STRUCT_H_
#include "Interval_struct.h"

enum Formula_type {FORMULA, DISCRETE, CONTINUOUS, COMBI, AND, NEG, UNTIL, TT, PROB, ADFORMULA};
enum Operator {LEQ, LESSER, GREATER, GEQ};

struct Formula {
public:
	Formula *leftChild;
	Formula *rightChild;
	double prob;
	Operator op;
	char* placeIndex;
	double constant;
	Interval bound;
	Formula_type type;
};

#endif /* FORMULA_STRUCT_H_ */
