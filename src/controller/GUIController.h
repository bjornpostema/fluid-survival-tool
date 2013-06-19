#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H

#include <QMainWindow>
#include <QString>
#include <QPlainTextEdit>
#include "../model/Logger.h"
#include <ios>
#include <fstream>
#include <sstream>
#include "STDDialogController.h"
#include "PlaceProbDialogController.h"
#include "ModelCheckDialogController.h"
#include <string.h>

namespace Ui {
class GUIController;
}

class GUIController : public QMainWindow, virtual public model::Logger
{
    Q_OBJECT
    
public:
    explicit GUIController(QWidget *parent = 0);
    ~GUIController();

    void addText(std::string str);
    void addSuccess(std::string str);
    void addWarning(std::string str);
    void addError(std::string str);
    void setText(std::string str);
    std::string getText();

protected:
    void closeEvent(QCloseEvent *event);
    
private:
    Ui::GUIController *ui;
    void readSettings();
    void writeSettings();
    bool checkSave(QTextEdit *editor);
    void openFile(const QString &fileName, QTextEdit *editor);
    bool saveFile(const QString &fileName, QTextEdit *editor);
    void modelSetCurrentFile(const QString &fileName);
    void modelOpen(QString filename);
    QString strippedName(const QString &fullFileName, QTextEdit *editor);
    std::string escapeHTML(std::string & Str);

    QString modelCurFile;
    QString modelFileName;

    int pc;

/* This is the Controller part of the class.
 * Slots receive signals from the view, while processing these slots to a model.
 */
private slots:
    void modelNew();
    void modelOpen();
    void exampleReservoir();
    void exampleWater();
    void exampleSewage();
    void exampleOverflow();
    bool modelSave();
    bool modelSaveAs();
    void modelModified();
    void about();
    void openProjectWebsite();
    void generateSTD();
    void generateProbFunc();
    void modelCheck();
//    void tempModelCheck();
};
#endif // GUICONTROLLER_H
