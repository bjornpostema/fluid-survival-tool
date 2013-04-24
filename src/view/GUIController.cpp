/**
 * @file FSTGui.cpp
 * @author B.F. Postema
 * @brief The GUI for the tool with some simple internal QT features.
 */
#include "GUIController.h"
#include "ui_GUIView.h"
//#include "../controller/GUIController.h"
#include "iostream"
#include <QtGui>
#include "QRect"
#include "QDesktopWidget"

/**
 * @brief FSTGui::FSTGui is the constructor of the main window.
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

GUIController::~GUIController()
{
    delete ui;
}

void GUIController::closeEvent(QCloseEvent *event)
{
    if (checkSave(ui->modelEditor) && checkSave(ui->specEditor)) {
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
    }
}

void GUIController::specNew()
{
    if (checkSave(ui->specEditor)) {
        ui->specEditor->clear();
        specSetCurrentFile("");
        specModified();
    }
}

void GUIController::modelOpen()
{
    if (checkSave(ui->modelEditor)) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open HPnG model"),"untitled.hpng",tr("HPnG files (*.hpng *.m);;All Files (*.*)"));
        if (!fileName.isEmpty())
            openFile(fileName,ui->modelEditor);
    }
}

void GUIController::specOpen()
{
    if (checkSave(ui->specEditor)) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open STL specification"),"untitled.stl",tr("STL files (*.stl);;All Files (*.*)"));
        if (!fileName.isEmpty())
            openFile(fileName,ui->specEditor);
    }
}

bool GUIController::modelSave()
{
    if (modelCurFile.isEmpty()) {
        return modelSaveAs();
    } else {
        return saveFile(modelCurFile,ui->modelEditor);
    }
}

bool GUIController::specSave()
{
    if (specCurFile.isEmpty()) {
        return specSaveAs();
    } else {
        return saveFile(specCurFile,ui->specEditor);
    }
}

bool GUIController::modelSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save HPnG model"),"untitled.hpng",tr("HPnG files (*.hpng *.m);;All Files (*.*)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName,ui->modelEditor);
}

bool GUIController::specSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save STL specification"),"untitled.stl",tr("STL files (*.stl);;All Files (*.*)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName,ui->specEditor);
}

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

void GUIController::modelModified()
{
    ui->modelFileName->setText((modelCurFile == "" ? "untitled.hpng" : modelFileName) + (ui->modelEditor->document()->isModified() ? "*":""));
}

void GUIController::specModified()
{
    ui->specFileName->setText((specCurFile == "" ? "untitled.stl" : specFileName) + (ui->specEditor->document()->isModified() ? "*":""));
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

void GUIController::openProjectWebsite() {
     if (QDesktopServices::openUrl(QUrl("https://code.google.com/p/fluid-survival-tool/", QUrl::TolerantMode))) {
         statusBar()->showMessage("Opened URL in default browser",2000);
     } else {
         statusBar()->showMessage("Could not open URL",2000);
     }
}

void GUIController::generatePrDistr() {
    //GUIController* gc = new GUIController();
    std::cout << "bp1" << std::endl;
    //gc->generatePrDistr(modelCurFile,ui->placeEdit->text());
}
