#include <QApplication>
#include "GUIController.h"
#include <iostream>

/**
 * @brief The initial method called.
 * This starts the GUI application and shows the mainWindow.
 * @param argc The amount of arguments given to this main function
 * @param argv A char for each of the arguments.
 * @return int
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("University of Twente");
    app.setApplicationName("Fluid Survival Tool");
    GUIController mainWindow;
    mainWindow.show();
    return app.exec();
}
