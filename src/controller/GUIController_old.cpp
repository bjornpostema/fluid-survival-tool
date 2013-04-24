#include "GUIController.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <QString>

extern "C" {
#include <matheval.h>
}

#include <cmath>
//#include <direct.h>

using namespace std;

//The messy helpers
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


int K = 9;

int fact(int n){
    if (n == 0)
        return 1;
    else
        return n*fact(n-1);
}

double gamma(double s){
    double sum = 0;
    for (int i = 0; i < K; i++){
        sum += (1/fact(i))*pow((s/lambda), i)*exp(-s/lambda);
    }
    return 1 - sum;
}

double sigma = 1;
double mu = 0;
double normal(double s){
    return -.5*(1 - erf((s - mu)/(sigma*1.414213562)));
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

GUIController::GUIController() {
}

GUIController::~GUIController() {
}

void GUIController::generatePrDistr(QString rawFileName, QString rawFluidPlaceName)
{
    Model *model;

    // Process the raw input of the user.
    QByteArray ba = rawFileName.toLatin1();
    const char *fileName = ba.data();
    QByteArray ba2 = rawFluidPlaceName.toLatin1();
    const char *fluidPlaceName = ba2.data();

    model = ReadModel(fileName);
std::cout << "bp2" << std::endl;
    unsigned int pIndex, disPIndex;
    ModelChecker modelChecker(model, TimedDiagram::getInstance());
    for (int i = 0; i < model->N_places; i++) {
        //if (model->places[i].type != PT_FLUID) continue;
        if (model->places[i].type == PT_FLUID && strncmp(model->places[i].id, fluidPlaceName, strlen(fluidPlaceName)) == 0)  pIndex = i;;
        if (model->places[i].type == PT_DISCRETE && strncmp(model->places[i].id, "Input1On", strlen("Input1On")) == 0)  disPIndex = i;;

    }

    unsigned int tIndex = 0;
    for (int i = 0; i < model->N_transitions; i++) {
            if (model->places[i].type != TT_DETERMINISTIC) continue;
            if (strncmp(model->transitions[i].id, "failure", strlen("failure")) != 0) continue;

            tIndex = i;
    }

    double time = model->transitions[tIndex].time;
    cout << "time: " <<  time << endl;
    cout << "pIndex: " <<  pIndex << endl;
    cout << "disPIndex: " <<  disPIndex << endl;


    InitializeModel(model);
    //Input and output list of places and transitions are created and sorted wrt to their priority and share.

    argument = model->transitions[gTransitionId(model)].df_argument;

    mu = time + 2;

    model->MaxTime = time + 50.0;
    Marking* initialMarking = createInitialMarking(model);

    long mtime, seconds, useconds;
std::cout << "bp3" << std::endl;
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


    TimedDiagram::getInstance()->scale = 20;
    std::cout << "Writing the debug region diagram...." << std::endl;
    std::stringstream ss;
    ss << fileName << "_rd";
    TimedDiagram::getInstance()->saveDiagram(ss.str());
    cv::Mat flipped;
    cv::flip(TimedDiagram::getInstance()->debugImage, flipped, 0);
    cv::imshow("test", flipped);
    cv::waitKey(0);

    AtomContFormula* psi1 = new AtomContFormula(pIndex, 0.1);

    AtomDisFormula* psi2_dis = new AtomDisFormula(disPIndex, 1);
    AtomContFormula* psi2_cont = new AtomContFormula(pIndex, 3);
    ADFormula* psi2 = new ADFormula(psi2_cont, psi2_dis);


    modelChecker.debugImage = TimedDiagram::getInstance()->debugImage;
    modelChecker.scale = TimedDiagram::getInstance()->scale;

    ofstream oFile("./results.out");
    ofstream oInt("./intervals.out");
    for (double T = 1; T < 11; T += 1){
        Interval bound(0, time+T);
        IntervalSet* i1 = modelChecker.until(psi1, psi2, bound, time);

        cout << "The satisfaction set of Until formula: " << endl;
        for (unsigned int i = 0; i < i1->intervals.size(); i++){
            i1->intervals[i].end = i1->intervals[i].end - time;
            i1->intervals[i].start = i1->intervals[i].start - time;
        }

        i1->print(oInt);

        oFile << modelChecker.calcProb(i1, normal, 0) << endl;
    }
}
