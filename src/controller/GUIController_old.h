#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H

#include "../model/DFPN2.h"
#include "../model/Line.h"
#include "../model/TimedDiagram.h"
#include "../model/IntervalSet.h"

#include "../model/ModelChecker.h"

#include <QString>

class GUIController
{
public:
    GUIController();
    virtual ~GUIController();

    virtual void generatePrDistr(QString rawFileName, QString rawFluidPlaceName);
};

#endif // GUICONTROLLER_H
