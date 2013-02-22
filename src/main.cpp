/*
 * main.cpp
 *
 *  Created on: 10 nov. 2011
 *      Author: GhasemiehH
 */

#include <iostream>
#include <sstream>
#include <cstring>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

extern "C" {
#include <matheval.h>
}
//#include <direct.h>

#include "DFPN2.h"
#include "Line.h"
#include "TimedDiagram.h"

#define MAX_ID_LEN 2048


//double expPrarameter = 10;
char *argument;
double lambda;
double a,b;
void *f;

/*
 * The exponential probability density function.
 */
double spdfExp(double s){
	return (1 - exp(-s/lambda));
}

/*
 * The uniform probability density function.
 */
double spdfUni(double s){
	return ((s-a)/(b-a));
}

/* The general probability density function defined by user.
The math library supports the following functionalities:
 *
 * -> e (e), log2(e) (log2e), log10(e) (log10e), ln(2) (ln2), ln(10) (ln10), pi (pi),
 * pi / 2 (pi_2), pi / 4 (pi_4), 1 / pi (1_pi), 2 / pi (2_pi), 2 / sqrt(pi) (2_sqrtpi),
 * sqrt(2) (sqrt) and sqrt(1 / 2) (sqrt1_2).
 * -> exponential (exp), logarithmic (log), square root (sqrt), sine (sin), cosine (cos),
 * tangent (tan), cotangent (cot), secant (sec), cosecant (csc), inverse sine (asin),
 * inverse cosine (acos), inverse tangent (atan), inverse cotangent (acot),
 * inverse secant (asec), inverse cosecant (acsc), hyperbolic sine (sinh),
 * cosine (cosh), hyperbolic tangent (tanh), hyperbolic cotangent (coth),
 * hyperbolic secant (sech), hyperbolic cosecant (csch), hyperbolic inverse sine (asinh),
 * hyperbolic inverse cosine (acosh), hyperbolic inverse tangent (atanh),
 * hyperbolic inverse cotangent (acoth), hyperbolic inverse secant (asech),
 * hyperbolic inverse cosecant (acsch), absolute value (abs),
 * Heaviside step function (step) with value 1 defined for x = 0,
 * Dirac delta function with infinity (delta) and not-a-number (nandelta)
 * values defined for x = 0, and error function (erf).
 * -> unary minus ('-'),addition ('+'), subtraction ('+'), multiplication ('*'),
 * division multiplication ('/') and exponentiation ('^')
 * -> Parenthesis ('(' and ')') could be used to change priority order
 *
 * For more details visit: http://www.gnu.org/software/libmatheval/manual/libmatheval.html
 */
double spdfGen(double s){
	char *names[] = { "s" };
	double values[] = { s };

	return evaluator_evaluate (f, sizeof(names)/sizeof(names[0]), names, values);
}


//the property: if Pf1 has more than 5 units of fluid.

int pIndex = 16;
double amount = 0;
bool propertyTest(Model* model, Marking* marking, double t0, double t1, double &s1, double &s2){
	/**
	 * fluid level in a place is (as+b) + (t1s+t0)d. t1s+t0 is time that has passed after entering this region.
	 */
	double b = marking->fluid0[model->places[pIndex].idInMarking] + t0*marking->fluidPlaceDeriv[model->places[pIndex].idInMarking];
	double a = marking->fluid1[model->places[pIndex].idInMarking] + t1*marking->fluidPlaceDeriv[model->places[pIndex].idInMarking];
	if (IS_ZERO(a)) {
		if (b <= amount) 
			return true;
		else 
			return false;
	}
	double p = (amount-b)/a;
	if (p < s2 && p > s1){
		if (a < -ZERO_PREC) s1 = p; 
		if (a > +ZERO_PREC) s2 = p;
		return true;
	} else if (p > s2){
		if (a * s2 + b < amount) return true;
	} else if (p < s1){
		if (a * s1 + b < amount) return true;
	}

	return false;
}



int main(int argc, char *argv[]) {
	Model *model;

	if (argc != 3) {
		//std::cout << "\n\n Error! Usage: DFPN ModelFile.m FluidPlaceName STD(optional, should be present if you want the STD as a JPG file.)" << std::endl;
		std::cout << "\n\n Error! Usage: DFPN ModelFile.m FluidPlaceName" << std::endl;
		return 0;
	}

	model = ReadModel(argv[1]);
	InitializeModel(model);
	//Input and output list of places and transitions are created and sorted wrt to their priority and share.

	argument = model->transitions[gTransitionId(model)].df_argument;

	model->MaxTime = 100.0;
	Marking* initialMarking = createInitialMarking(model);

	long mtime, seconds, useconds;

	TimedDiagram::getInstance()->setModel(model);

	timeval gt1, gt2;
	gettimeofday(&gt1, NULL);
	TimedDiagram::getInstance()->generateDiagram(initialMarking);
	gettimeofday(&gt2, NULL);

	seconds  = gt2.tv_sec  - gt1.tv_sec;
	useconds = gt2.tv_usec - gt1.tv_usec;
	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

	std::cout << "Number of regions: " <<TimedDiagram::getInstance()->getNumberOfRegions() << std::endl;
	std::cout << "Time to generate STD: " << mtime<< "ms" << std::endl;


//	if (argc == 4){
//		std::cout << "Writing the debug region diagram...." << std::endl;
//		std::stringstream ss;
//		ss << argv[1] << "_rd";
//		TimedDiagram::getInstance()->saveDiagram(ss.str());
//	}

	std::cout << "starting measure computation..." << std::endl;

	//Model* model = TimedDiagram::getInstance()->model;
	for (int i = 0; i < model->N_places; i++) {
		if (model->places[i].type != PT_FLUID) continue;
		if (strncmp(model->places[i].id, argv[2], strlen(argv[2])) != 0) continue;

		pIndex = i;
	}

	std::ofstream oFile;
	oFile.open(argv[2], std::ios::out);

	mtime = 0; seconds = 0; useconds = 0;
	timeval t0, t1;
	switch (model->transitions[gTransitionId(model)].df_distr)
	{
	case Exp: lambda = atoi(argument);
	break;
	case Uni:
		char *argFinder;
		argFinder = strtok(argument,",");
		a = atoi(argFinder);
		argFinder = strtok(NULL,",");
		b = atoi(argFinder);
	break;
	case Gen:
		f = evaluator_create (model->transitions[gTransitionId(model)].df_argument);
	break;
	}
	for (double t = .02; t <= model->MaxTime + .01; t += .5){
		for (amount = .05; amount <= model->places[pIndex].f_bound; amount += .2){
			//std::cout << "t = " << t << "--amount = " << amount << std::endl;
			double p = -1.0;
			gettimeofday(&t0, NULL);
			//double p = TimedDiagram::getInstance()->calProbAtTime(t, spdf, propertyTest);
			switch (model->transitions[gTransitionId(model)].df_distr)
			{
			case Exp: p = TimedDiagram::getInstance()->calProbAtTime(t, spdfExp, propertyTest);
			break;
			case Uni: p = TimedDiagram::getInstance()->calProbAtTime(t, spdfUni, propertyTest);
			break;
			case Gen: p = TimedDiagram::getInstance()->calProbAtTime(t, spdfGen, propertyTest);
			break;
			}
			gettimeofday(&t1, NULL);

			seconds  += t1.tv_sec  - t0.tv_sec;
			useconds += t1.tv_usec - t0.tv_usec;

			oFile << " "<< p;
			//std::cout << " "<< p;
		}
		oFile << std::endl;
		//std::cout << std::endl;
	}

	switch (model->transitions[gTransitionId(model)].df_distr)
	{
	case Gen: evaluator_destroy(f);
	break;
	}
	mtime = ((seconds * 1000 + useconds/1000.0) + 0.5);
	std::cout << "total time computing measures:  " << mtime << "ms" << std::endl;

	oFile.close();
	FILE* gnuplotPipe;
	if ( (gnuplotPipe = popen("gnuplot -persist","w")) )
	{
		fprintf(gnuplotPipe,"set pm3d \n unset surface\n "
				"set xlabel \"Storage Content [m^3]\" \n"
				"set zlabel \"Probability\"\n"
				"set ylabel \"time [hours]\"\n"
				"set xtics (\"0\" 0, \"2\" 10, \"4\" 20,\"6\" 30, \"8\" 40) \n "
				"set ytics (\"0\" 0, \"20\" 40, \"40\" 80,\"60\" 120, \"80\" 160, \"100\" 200) \n "
				"set palette rgbformulae 33,13,10 \n");

		fflush(gnuplotPipe);
		fprintf(gnuplotPipe,"splot \"%s\" matrix with lines\n",argv[2]);
		fflush(gnuplotPipe);
		fprintf(gnuplotPipe,"exit \n");
		fclose(gnuplotPipe);
	}




	//getch();


	return 0;
}

//int main(int argc, char *argv[]) {
//	Model *model;
//
//	if (argc != 2) {
//		std::cout << "\n\n Error! Usage: DFPN ModelFile.m " << std::endl;
//		return 0;
//	}
//
//	model = ReadModel(argv[1]);
//	InitializeModel(model);
//	//Input and output list of places and transitions are created and sorted wrt to their priority and sahre.
//
//	model->MaxTime = 100.0;
//	Marking* initialMarking = createInitialMarking(model);
//
//
//
//	TimedDiagram::getInstance()->setModel(model);
//
//	clock_t gt1, gt2;
//	gt1= clock();
//	TimedDiagram::getInstance()->generateDiagram(initialMarking);
//	gt2 = clock();
//
//	std::cout << "Number of regions: " <<TimedDiagram::getInstance()->getNumberOfRegions() << std::endl;
//	std::cout << "Cmp of regions: " <<TimedDiagram::getInstance()->getNumberOfRegions() << std::endl;
//
//
//	std::cout << "Writting the debug region diagram...." << std::endl;
//	std::stringstream ss;
//	ss << argv[1] << "_rd";
//	//TimedDiagram::getInstance()->saveDiagram(ss.str());
//
//	clock_t time = 0;
//	clock_t t0, t1;
//
//	std::cout << "starting measure computation..." << std::endl;
//
//	//Model* model = TimedDiagram::getInstance()->model;
//	for (int i = 0; i < model->N_places; i++) {
//		if (model->places[i].type != PT_FLUID) continue;
//		if (strncmp(model->places[i].id, "stor", strlen("stor")) != 0) continue;
//
//		pIndex = i;
//		std::stringstream name;
//		name << argv[1] << "_" << model->places[i].id << ".out";
//		std::ofstream oFile;
//		oFile.open(name.str().c_str(), std::ios::out);
//		//pIndex = 2;
//		for (double t = .02; t <= model->MaxTime + .01; t += .5){
//			for (amount = .05; amount <= model->places[i].f_bound; amount += .2){
//				//std::cout << "t = " << t << "--amount = " << amount << std::endl;
//				t0 = clock();
//				double p = TimedDiagram::getInstance()->calProbAtTime(t, spdf, propertyTest);
//				t1 = clock();
//
//				time += (t1 - t0);
//
//				oFile << " "<< p;
//				//std::cout << " "<< p;
//			}
//			oFile << std::endl;
//			//std::cout << std::endl;
//		}
//
//		oFile.close();
//		FILE* gnuplotPipe;
//		if (gnuplotPipe = popen("gnuplot -persist","w"))
//		{
//			fprintf(gnuplotPipe,"set pm3d \n unset surface\n "
//					"set xlabel \"Storage Content [m^3]\" \n"
//					"set zlabel \"Probability\"\n"
//					"set ylabel \"time [hours]\"\n"
//					"set xtics (\"0\" 0, \"2\" 10, \"4\" 20,\"6\" 30, \"8\" 40) \n "
//					"set ytics (\"0\" 0, \"20\" 40, \"40\" 80,\"60\" 120, \"80\" 160, \"100\" 200) \n "
//					"set palette rgbformulae 33,13,10 \n");
//
//			fflush(gnuplotPipe);
//			fprintf(gnuplotPipe,"splot \"%s\" matrix with lines\n",name.str().c_str());
//			fflush(gnuplotPipe);
//			fprintf(gnuplotPipe,"exit \n");
//			fclose(gnuplotPipe);
//		}
//	}
//
//
//
//	std::cout << "total time computing measures:  " << 100*time << "ms" << std::endl;
//
//	//getch();
//
//
//	return 0;
//}



//int main(int argc, char *argv[]) {
//	Model *model;
//
//	std::string dir = "D:\\Profiles\\GhasemiehH\\Scientific\\UTwente\\Research\\DFPN\\DFPN\\Test\\rate\\";
//	std::string fpreffix = "dsn_";
//	std::string fSuffix = "rates.m";
//	std::stringstream file;
//	std::stringstream ss;
//	std::stringstream name;
//
//	file << dir << "result.txt";
//	std::ofstream outFile (file.str());
//
//	for (int l = 2; l < 12; l++){
//
//		outFile << l << "\t";
//
//		file.str("");
//		if ( l==11)
//			file << dir << fpreffix << 2 << fSuffix;
//		else
//			file << dir << fpreffix << l << fSuffix;
//		model = ReadModel(file.str().c_str());
//		InitializeModel(model);
//		//Input and output list of places and transitions are created and sorted wrt to their priority and sahre.
//
//		model->MaxTime = 100.0;
//		Marking* initialMarking = createInitialMarking(model);
//
//
//
//		TimedDiagram::getInstance()->setModel(model);
//
//		clock_t gt1, gt2;
//		gt1= clock();
//		TimedDiagram::getInstance()->generateDiagram(initialMarking);
//		gt2 = clock();
//
//		outFile << TimedDiagram::getInstance()->getNumberOfRegions() << "\t" << gt2 - gt1 << "\t";
//
//
//		std::cout << "Writting the debug region diagram...." << std::endl;
//		ss.str("");
//		ss << file.str() << "_rd";
//		//TimedDiagram::getInstance()->saveDiagram(ss.str());
//
//		clock_t time = 0;
//		clock_t t0, t1;
//
//		std::cout << "starting measure computation..." << std::endl;
//
//		//Model* model = TimedDiagram::getInstance()->model;
//		for (int i = 0; i < model->N_places; i++) {
//			if (model->places[i].type != PT_FLUID) continue;
//			if (strncmp(model->places[i].id, "stor", strlen("stor")) != 0) continue;
//
//			pIndex = i;
//			name.str("");
//			name << file.str() << "_" << model->places[i].id << ".out";
//			std::ofstream oFile = std::ofstream(name.str());
//			//pIndex = 2;
//			for (double t = 0.01; t < model->MaxTime + .001 ; t += .5){
//				for (amount = 0; amount <= model->places[i].f_bound; amount += .2){
//					//std::cout << "t = " << t << "--amount = " << amount << std::endl;
//					t0 = clock();
//					double p = TimedDiagram::getInstance()->calProbAtTime(t, spdf, propertyTest);
//					t1 = clock();
//
//					time += (t1 - t0);
//
//					oFile << " "<< p;
//					//std::cout << " "<< p;
//				}
//				oFile << std::endl;
//				//std::cout << std::endl;
//			}
//		}
//
//		outFile << time << std::endl;
//	}
//
//	outFile.close();
//	getch();
//	return 0;
//}
//

