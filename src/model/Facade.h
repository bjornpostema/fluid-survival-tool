
#ifndef FACADE_H
#define FACADE_H

#include "DFPN2.h"
#include "TimedDiagram.h"
#include "Logger.h"
#include "Formula.h"
#include "ModelChecker.h"

#include <QString>
#include <QTextEdit>
#include <cmath>
#include <iostream>
#include <ostream>
#include <sstream>
#include <cstring>
#include <fstream>
#include <sys/time.h>

namespace model {

class Facade
{
public:
    Facade(QString rawFileName, QString rawPlaceName, Logger *guic);
    Facade(QString rawFileName, Logger *guic);
    virtual ~Facade();
    bool virtual setPlace(QString rawPlaceName);
    bool virtual setFile(QString rawFileName);
    bool virtual showSTD(QString rawFileName, double maxTime, int imageScale);
    bool virtual showProbFunc(double cStart, double cEnd, double cStep, double tStep, double maxTime);
    bool virtual showProbFunc(double c, double tStep, double maxTime);
    bool virtual modelCheck(bool &res, QString rawFormula, QString rawCheckTime, double maxTime);
//    bool virtual tempUntilModelCheck();

    Model *model;
    QString fileName;
    QString placeName;
    Logger *guic;

private:
    const char* QString2Char(QString rawQString);
};

}

#endif // FACADE_H
