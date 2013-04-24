/**
 * @file FSTGui.cpp
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
#include "../model/Calculator.h"

/**
 * @brief GUIController::GUIController is the constructor of the main window.
 * The main window constructs all the elements via the ui file.
 * The title of the main window is set to the tool name.
 * The position of the main window is adjusted to the center.
 * The default empty files are initiated.
 *
 * @param parent the parent QWidget
 */
GUIController::GUIController(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUIController)
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
    specSetCurrentFile("");

    // Since it might look nice.
    setUnifiedTitleAndToolBarOnMac(true);

    // Initialization procedure was succesful.
    ui->statusBar->showMessage("Ready");
}

/**
 * @brief GUIController::~GUIController The destroyer that frees the memory
 * The ui is deleted and the memory is freed.
 */
GUIController::~GUIController()
{
    delete ui;
}

/**
 * @brief GUIController::closeEvent Check if the files are saved when the window is closed.
 * @param[out] event The closing event.
 */
void GUIController::closeEvent(QCloseEvent *event)
{
    if (checkSave(ui->modelEditor) && checkSave(ui->specEditor)) {
        event->accept();
    } else {
        event->ignore();
    }
}

/**
 * @brief GUIController::modelNew Creates a new model file in the model text editor.
 */
void GUIController::modelNew()
{
    if (checkSave(ui->modelEditor)) {
        ui->modelEditor->clear();
        modelSetCurrentFile("");
        modelModified();
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->ModelTab));
    }
}

/**
 * @brief GUIController::specNew Creates a new specification file in the specification text editor
 */
void GUIController::specNew()
{
    if (checkSave(ui->specEditor)) {
        ui->specEditor->clear();
        specSetCurrentFile("");
        specModified();
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->SpecTab));
    }
}

/**
 * @brief GUIController::modelOpen Ask to define a file name for the model and open the file.
 */
void GUIController::modelOpen()
{
    if (checkSave(ui->modelEditor)) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open HPnG model"),"untitled.hpng",tr("HPnG files (*.hpng *.m);;All Files (*.*)"));
        if (!fileName.isEmpty()) {
            openFile(fileName,ui->modelEditor);
            ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->ModelTab));
        }
    }
}

/**
 * @brief GUIController::specOpen Ask to define a file name for the specification and open the file.
 */
void GUIController::specOpen()
{
    if (checkSave(ui->specEditor)) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open STL specification"),"untitled.stl",tr("STL files (*.stl);;All Files (*.*)"));
        if (!fileName.isEmpty()) {
            openFile(fileName,ui->specEditor);
            ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->SpecTab));
        }
    }
}

/**
 * @brief GUIController::modelSave Saves the current model file.
 * @return Gives true if the model file is saved else false.
 */
bool GUIController::modelSave()
{
    ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->ModelTab));
    if (modelCurFile.isEmpty()) {
        return modelSaveAs();
    } else {
        return saveFile(modelCurFile,ui->modelEditor);
    }
}

/**
 * @brief GUIController::specSave Saves the current specification file.
 * @return Gives true if the specification file is saved else false.
 */
bool GUIController::specSave()
{
    ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->SpecTab));
    if (specCurFile.isEmpty()) {
        return specSaveAs();
    } else {
        return saveFile(specCurFile,ui->specEditor);
    }
}

/**
 * @brief GUIController::modelSaveAs Gives a dialog where to save the model file and saves the file.
 * @return Gives true if the model file is saved else false.
 */
bool GUIController::modelSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save HPnG model"),"untitled.hpng",tr("HPnG files (*.hpng *.m);;All Files (*.*)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName,ui->modelEditor);
}

/**
 * @brief GUIController::specSaveAs Gives a dialog where to save the specification file and saves the file.
 * @return Gives true if the specification file is saved else false.
 */
bool GUIController::specSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save STL specification"),"untitled.stl",tr("STL files (*.stl);;All Files (*.*)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName,ui->specEditor);
}

/**
 * @brief GUIController::about Gives some information about the tool in a small about pop-up.
 */
void GUIController::about()
{
   QMessageBox::about(this, tr("About FST"),
            tr("FST is developed at the University of Twente.\n"
               "The application is for academic and non-commercial use only.\n\n"
               "Tool contributors:\n"
               "H. Ghasemieh, MSc.\n"
               "Dr. A.K.I. Remke\n"
               "B.F. Postema, BSc.\n"));
}

/**
 * @brief GUIController::modelModified Places an asterix near the filename to indicate that the model file was modified.
 */
void GUIController::modelModified()
{
    ui->modelFileName->setText((modelCurFile == "" ? "untitled.hpng" : modelFileName) + (ui->modelEditor->document()->isModified() ? "*":""));
}

/**
 * @brief GUIController::specModified Places an asterix near the filename to indicate that the specificaiton file was modified.
 */
void GUIController::specModified()
{
    ui->specFileName->setText((specCurFile == "" ? "untitled.stl" : specFileName) + (ui->specEditor->document()->isModified() ? "*":""));
}

/**
 * @brief GUIController::checkSave Saves a modified file from an text editor.
 * @param[in] editor The editor that should be saved.
 * @return Gives true when the file is saved else false.
 */
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
            } else if (editor == ui->specEditor) {
                return specSave();
            } else {
                return true;
            }
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

/**
 * @brief GUIController::openFile Opens the file of a specific filename and text editor.
 * @param[in] fileName The name of the file that should be opened.
 * @param[out] editor The editor that get the file content.
 */
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
    } else if (editor == ui->specEditor) {
        specSetCurrentFile(fileName);
        specModified();
    }
    statusBar()->showMessage(tr("File loaded"), 2000);
}

/**
 * @brief GUIController::saveFile Saves the file of a specific filename and text editor.
 * @param[out] fileName The name of the file that should be saved.
 * @param[in] editor The editor that contains the content that should be saved.
 * @return Gives true when the file is saved else false.
 */
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
#ifndef QT_NO_CFSTGuiURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << editor->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    if (editor == ui->modelEditor) {
        modelSetCurrentFile(fileName);
        modelModified();
    } else if (editor == ui->specEditor) {
        specSetCurrentFile(fileName);
        specModified();
    }
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

/**
 * @brief GUIController::modelSetCurrentFile Displays the current model file name.
 * @param fileName The name of the current model file.
 */
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

/**
 * @brief GUIController::specSetCurrentFile Displays the current specification file name.
 * @param fileName The name of the current specification file.
 */
void GUIController::specSetCurrentFile(const QString &fileName)
{
    specCurFile = fileName;
    ui->specEditor->document()->setModified(false);
    setWindowModified(false);

    QString shownName = specCurFile;
    if (specCurFile.isEmpty())
        shownName = "untitled.stl";
    setWindowFilePath(shownName);

    QStringList parts = fileName.split("/");
    specFileName = parts.at(parts.size()-1);
}

/**
 * @brief GUIController::openProjectWebsite Opens the googlecode project website.
 */
void GUIController::openProjectWebsite() {
     if (QDesktopServices::openUrl(QUrl("https://code.google.com/p/fluid-survival-tool/", QUrl::TolerantMode))) {
         statusBar()->showMessage("Opened URL in default browser",2000);
     } else {
         statusBar()->showMessage("Could not open URL",2000);
     }
}

/**
 * @brief GUIController::generateSTD Generates an STD.
 */
void GUIController::generateSTD() {
    try {
        this->addText("");
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->MCTab));
        if (checkSave(ui->modelEditor) && !modelCurFile.isEmpty()) {
            model::Calculator *c;
            c = new model::Calculator(modelCurFile,ui->placeEdit->text(),this);
            if (c->showSTD(modelFileName)) {
                this->addText("Displaying STD...");
            } else {
                this->addText("The STD could not be displayed.");
            }
        } else {
            this->addText("The model file should be saved before execution.");
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

/**
 * @brief GUIController::generateProbFunc Generates an probability function for a given place.
 */
void GUIController::generateProbFunc() {
      try {
        // do something, maybe in a loop within subroutines
        // that will trow specific errors for instance DecodingError
        this->addText("");
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->MCTab));
        if (!modelCurFile.isEmpty() && checkSave(ui->modelEditor)) {
            model::Calculator *c;
            c = new model::Calculator(modelCurFile,ui->placeEdit->text(),this);
            if (c->showProbFunc()) {
                this->addText("Displaying Probability Distribution Pr-t plot...");
            } else {
                this->addText("The Probability Distribution Pr-t plot could not be displayed.");
            }
        } else {
            this->addText("The model file should be saved before execution.");
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

void GUIController::modelCheck(){
    try {
        this->addText("");
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->MCTab));
        if (!modelCurFile.isEmpty() && checkSave(ui->modelEditor)) {
            model::Calculator *c;
            c = new model::Calculator(modelCurFile,ui->placeEdit->text(),this);
            if (c->modelCheck(ui->specEditor->toPlainText(), ui->time2Check->text())) {
                this->addText("Model checking...");
            } else {
                this->addText("The model checking could not be performed.");
            }
        } else {
            this->addText("The model file and the spec file should be saved before execution.");
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

/**
 * @brief GUIController::addText Add a text to the build-in terminal
 * @param str String containing the text to be added.
 */
void GUIController::addText(QString str)
{
    //this->setText("<font color=\"Black\"> \>" + str + "</font>" + "\n" + "<font color=\"Grey\">" + this->getText() + "</font>");
    this->setText(this->getText() + "\n" + str);
}

/**
 * @brief GUIController::setText Set the text in the build-in terminal
 * @param str String containing the text to be set.
 */
void GUIController::setText(QString str)
{
    //ui->outputEditor->setHtml(str);
    ui->outputEditor->setText(str);
    QScrollBar *sb = ui->outputEditor->verticalScrollBar();
    sb->triggerAction(QScrollBar::SliderToMaximum);
}

/**
 * @brief GUIController::getText Give the current text of the build-in terminal
 * @return Gives a QString text of the build-in terminal.
 */
QString GUIController::getText()
{
    return ui->outputEditor->toPlainText();
}
