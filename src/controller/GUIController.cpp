/**
 * @file GUIController.cpp
 * @author B.F. Postema
 * @brief The GUI for the tool with some simple internal QT features.
 */
#include "GUIController.h"
#include "ui_GUIView.h"
#include "../model/Polygon.h"
#include "iostream"
#include <QtGui>
#include "QRect"
#include "QDesktopWidget"
#include "../model/Facade.h"
#include "../model/Logger.h"

GUIController::GUIController(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUIController),
    Logger()
{
    // Create all elements in the window from the UI file.
    ui->setupUi(this);

    // Adjust the title of the window.
    this->setWindowTitle("Fluid Survival Tool");

    // Move windows to the center of the screen.
    QRect position = frameGeometry();
    position.moveCenter(QDesktopWidget().availableGeometry().center());
    move(position.topLeft());

    // Set the file names of the model and the specification.
    modelSetCurrentFile("");

    // Since it might look nice.
    setUnifiedTitleAndToolBarOnMac(true);

    // Initialization procedure was succesful.
    ui->statusBar->showMessage("Ready");

    // Set program counter to 0
    pc = 0;
}

GUIController::~GUIController()
{
    delete ui;
}

void GUIController::closeEvent(QCloseEvent *event)
{
    if (checkSave(ui->modelEditor)) {
        event->accept();
    } else {
        event->ignore();
    }
}

void GUIController::modelNew()
{
    if (checkSave(ui->modelEditor)) {
        ui->modelEditor->clear();
        modelSetCurrentFile("");
        modelModified();
        this->addSuccess("New model file created.");
    }
}

///**
// * @brief GUIController::specNew Creates a new specification file in the specification text editor
// */
//void GUIController::specNew()
//{
//    if (checkSave(ui->specEditor)) {
//        ui->specEditor->clear();
//        specSetCurrentFile("");
//        specModified();
//        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->SpecTab));
//    }
//}

void GUIController::modelOpen()
{
    if (checkSave(ui->modelEditor)) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open HPnG model"),"untitled.hpng",tr("HPnG files (*.hpng *.m);;All Files (*.*)"));
        if (!fileName.isEmpty()) {
            openFile(fileName,ui->modelEditor);
            this->addSuccess("Model file loaded.");
        }
    }
}

void GUIController::modelOpen(QString fileName)
{
    if (checkSave(ui->modelEditor)) {
        if (!fileName.isEmpty()) {
            openFile(fileName,ui->modelEditor);
            this->addSuccess("Model file loaded.");
        }
    }
}

void GUIController::exampleReservoir()
{
    this->addText("Loading Reservoir model file...");
    this->modelOpen("./examples/reservoir.hpng");
}

void GUIController::exampleWater()
{
    this->addText("Loading Water cleaning facility model file...");
    this->modelOpen("./examples/dsn_2filter.hpng");
}

void GUIController::exampleSewage()
{
    this->addText("Loading Sewage water cleaning model file...");
    this->modelOpen("./examples/overstroombasis.hpng");
}

void GUIController::exampleOverflow()
{
    this->addText("Loading Overflow model file...");
    this->modelOpen("./examples/overflow.hpng");
}

bool GUIController::modelSave()
{
    bool res = false;
    if (modelCurFile.isEmpty()) {
        res = modelSaveAs();
    } else {
        res = saveFile(modelCurFile,ui->modelEditor);
    }

    res ? this->addSuccess("Model file is succesfully saved.") : this->addError("Model file is NOT saved.");
    return res;
}

bool GUIController::modelSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save HPnG model"),"untitled.hpng",tr("HPnG files (*.hpng *.m);;All Files (*.*)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName,ui->modelEditor);
}

void GUIController::about()
{
   QMessageBox::about(this, tr("About FST"),
            tr("Fluid Survival Tool is developed at the University of Twente.\n"
               "The application is for academic and non-commercial use only.\n\n"
               "Tool contributors:\n"
               "B.F. Postema, BSc.\n"
               "H. Ghasemieh, MSc.\n"
               "Dr. A.K.I. Remke\n"));
}

void GUIController::modelModified()
{
    ui->modelFileName->setText((modelCurFile == "" ? "untitled.hpng" : modelFileName) + (ui->modelEditor->document()->isModified() ? "*":""));
}

bool GUIController::checkSave(QTextEdit *editor)
{
    if (editor->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            if (editor == ui->modelEditor) {
                return modelSave();
            } else {
                return true;
            }
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void GUIController::openFile(const QString &fileName, QTextEdit *editor)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    editor->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    if (editor == ui->modelEditor) {
        modelSetCurrentFile(fileName);
        modelModified();
    }
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool GUIController::saveFile(const QString &fileName, QTextEdit *editor)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << editor->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    if (editor == ui->modelEditor) {
        modelSetCurrentFile(fileName);
        modelModified();
    }
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void GUIController::modelSetCurrentFile(const QString &fileName)
{
    modelCurFile = fileName;
    ui->modelEditor->document()->setModified(false);
    setWindowModified(false);

    QString shownName = modelCurFile;
    if (modelCurFile.isEmpty())
        shownName = "untitled.hpng";
    setWindowFilePath(shownName);

    QStringList parts = fileName.split("/");
    modelFileName = parts.at(parts.size()-1);
}

void GUIController::openProjectWebsite() {
     if (QDesktopServices::openUrl(QUrl("https://code.google.com/p/fluid-survival-tool/", QUrl::TolerantMode))) {
         statusBar()->showMessage("Opened URL in default browser",2000);
     } else {
         statusBar()->showMessage("Could not open URL",2000);
     }
}

void GUIController::generateSTD() {
    STDDialogController dialogSTD(this,modelFileName);
    if (!checkSave(ui->modelEditor) || modelCurFile.isEmpty()) {
        this->addError("The model file must be saved before execution.");
    } else if(dialogSTD.exec() == QDialog::Accepted )
    {
        try {
            model::Facade *f;
            f = new model::Facade(modelCurFile,this);
            if (f->showSTD(modelFileName, dialogSTD.getMaxTime(), dialogSTD.getImageScale())) {
                this->addSuccess("Displaying STD...");
            } else {
                this->addError("The STD could not be displayed.");
            }
        } catch (const std::exception & e)  {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Exception.");
            msgBox.setInformativeText(e.what());
            msgBox.exec();
        } catch (...) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Exception.");
            msgBox.setInformativeText("An unknown exception is caught.");
            msgBox.exec();
        }
    }
}

void GUIController::generateProbFunc() {
    PlaceProbDialogController dialogPlaceProb(this,modelFileName);
    if (!checkSave(ui->modelEditor) || modelCurFile.isEmpty()) {
        this->addError("The model file must be saved before execution.");
    } else if(dialogPlaceProb.exec() == QDialog::Accepted )
    {
        try {
            if (dialogPlaceProb.checkConstRange()) {
                model::Facade *f;
                f = new model::Facade(modelCurFile,dialogPlaceProb.getPlaceName(),this);
                if (f->showProbFunc(dialogPlaceProb.getConstStart(),dialogPlaceProb.getConstEnd(),dialogPlaceProb.getConstStep(),dialogPlaceProb.getTimeStep(),dialogPlaceProb.getMaxTime())) {
                    this->addSuccess("Displaying Probability Distribution Pr-t plot...");
                } else {
                    this->addError("The Probability Distribution Pr-t plot could not be displayed.");
                }
            } if (dialogPlaceProb.checkSpecConst()) {
                model::Facade *f;
                f = new model::Facade(modelCurFile,dialogPlaceProb.getPlaceName(),this);
                if (f->showProbFunc(dialogPlaceProb.getConst(),dialogPlaceProb.getTimeStep(),dialogPlaceProb.getMaxTime())) {
                    this->addSuccess("Displaying Probability Distribution Pr-t plot...");
                } else {
                    this->addError("The Probability Distribution Pr-t plot could not be displayed.");
                }
            }
        } catch (const std::exception & e)  {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Exception.");
            msgBox.setInformativeText(e.what());
            msgBox.exec();
        } catch (...) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Exception.");
            msgBox.setInformativeText("An unknown exception is caught.");
            msgBox.exec();
        }
    }
}

void GUIController::modelCheck(){
    ModelCheckDialogController dialogModelCheck(this,modelFileName);
    if (!checkSave(ui->modelEditor) || modelCurFile.isEmpty()) {
        this->addError("The model file must be saved before execution.");
    } else if(dialogModelCheck.exec() == QDialog::Accepted )
    {
        this->addText("Model Checking procedure initialized.");
        try {
            model::Facade *f;
            f = new model::Facade(modelCurFile,this);
            bool res;
            if (f->modelCheck(res, dialogModelCheck.getFormula(), dialogModelCheck.getTTC(),dialogModelCheck.getMaxTime())) {
                this->addSuccess("Model checking is done.");
                QMessageBox msg(this);
                msg.setWindowTitle(QString("Model Checking Verdict"));
                if (res) {
                    msg.setWindowIcon(QPixmap(":/resources/resources/yes.png"));
                    msg.setIconPixmap(QPixmap(":/resources/resources/yes.png"));
                    msg.setInformativeText(QString("Yes! The formula %1 is satisfied.").arg(dialogModelCheck.getFormula()));
                } else {
                    msg.setWindowIcon(QPixmap(":/resources/resources/no.png"));
                    msg.setIconPixmap(QPixmap(":/resources/resources/no.png"));
                    msg.setInformativeText(QString("No! The formula %1 is not satisfied.").arg(dialogModelCheck.getFormula()));
                }
                QSpacerItem* horizontalSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
                QGridLayout* layout = (QGridLayout*)msg.layout();
                layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
                msg.exec();
            } else {
                this->addError("The model checking could not be performed.");
            }
        } catch (const std::exception & e)  {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Exception.");
            msgBox.setInformativeText(e.what());
            msgBox.exec();
        } catch (...) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Exception.");
            msgBox.setInformativeText("An unknown exception is caught.");
            msgBox.exec();
        }
    }
}

//void GUIController::tempModelCheck(){
//    ModelCheckDialogController dialogModelCheck(this);
//    if (!checkSave(ui->modelEditor) || modelCurFile.isEmpty()) {
//        this->addText("The model file should be saved before execution.");
//    } else if(dialogModelCheck.exec() == QDialog::Accepted )
//    {
//        try {
//            this->addText("");
//            model::Facade *f;
//            f = new model::Facade(modelCurFile,dialogModelCheck.getTempPlaceName(),this);
//            if (f->tempUntilModelCheck()) {
//                this->addText("The model checking process ended succesfully.");
//            } else {
//                this->addText("The model checking could not be performed.");
//            }
//        } catch (const std::exception & e)  {
//            QMessageBox msgBox;
//            msgBox.setWindowTitle("Exception.");
//            msgBox.setInformativeText(e.what());
//            msgBox.exec();
//        } catch (...) {
//            QMessageBox msgBox;
//            msgBox.setWindowTitle("Exception.");
//            msgBox.setInformativeText("An unknown exception is caught.");
//            msgBox.exec();
//        }
//    }
//}

/**
 * @brief GUIController::addText Add a normal text to the build-in terminal
 * @param str String containing the text to be added.
 */
void GUIController::addText(std::string str)
{
    std::stringstream sstream;
    sstream << pc;
    this->setText(this->getText() + "<b>" + sstream.str() + "</b> : " + escapeHTML(str));
    pc++;
}

/**
 * @brief GUIController::addText Add an information text to the build-in terminal
 * @param str String containing the text to be added.
 */
void GUIController::addSuccess(std::string str)
{
    std::stringstream sstream;
    sstream << pc;
    this->setText(this->getText() + "<b>" + sstream.str() + "</b> : " + "<font color=\"#4F8A10\">" + escapeHTML(str) + "</font>");
    pc++;
}

/**
 * @brief GUIController::addText Add a warning text to the build-in terminal
 * @param str String containing the text to be added.
 */
void GUIController::addWarning(std::string str)
{
    std::stringstream sstream;
    sstream << pc;
    this->setText(this->getText() + "<b>" + sstream.str() + "</b> : " + "<font color=\"#9F6000\">" + escapeHTML(str) + "</font>");
    pc++;
}

/**
 * @brief GUIController::addText Add an error text to the build-in terminal
 * @param str String containing the text to be added.
 */
void GUIController::addError(std::string str)
{
    std::stringstream sstream;
    sstream << pc;
    this->setText(this->getText() + "<b>" + sstream.str() + "</b> : " + "<font color=\"#D8000C\">" + escapeHTML(str) + "</font>");
    pc++;
}

void GUIController::setText(std::string str)
{
    QString qStr = QString::fromAscii(str.data(),str.size());
    ui->outputEditor->setHtml(qStr);
    QScrollBar *sb = ui->outputEditor->verticalScrollBar();
    sb->triggerAction(QScrollBar::SliderToMaximum);
}

std::string GUIController::getText()
{
    return ui->outputEditor->toHtml().toStdString();
}

std::string GUIController::escapeHTML(std::string & Str)
  /* returns Str with all characters with special HTML meanings converted to
    entity references. */
  {
    std::string Escaped="";
    for (int i = 0; i < Str.size(); ++i)
      {
        std::string ThisCh = Str.substr(i,1);
        if (ThisCh == "&")
            ThisCh = "&amp;";
        else if (ThisCh == "<")
            ThisCh = "&lt;";
        else if (ThisCh == "\"")
            ThisCh = "&quot;";
        else if (ThisCh == ">")
            ThisCh = "&gt;";
        Escaped += ThisCh;
      } /*for*/
    return Escaped;
  } /*EscapeHTML*/
