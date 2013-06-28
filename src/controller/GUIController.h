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

/**
 * @file GUIController.h
 * @author B.F. Postema
 * @brief The GUIController class is a controller that reacts to events from the model and the view.
 * When a user interacts with one of the objects the GUIController takes care of everything.
 */
class GUIController : public QMainWindow, virtual public model::Logger
{
    Q_OBJECT
    
public:
    /**
     * @brief GUIController is the constructor of the main window.
     * The main window constructs all the elements via the ui file.
     * The title of the main window is set to the tool name.
     * The position of the main window is adjusted to the center.
     * The default empty files are initiated.
     *
     * @param parent the parent QWidget
     */
    explicit GUIController(QWidget *parent = 0);
    /**
     * @brief The destroyer that frees the memory
     * The ui is deleted and the memory is freed.
     */
    ~GUIController();

    /**
     * @brief Add a normal text to the build-in terminal
     * @param str String containing the text to be added.
     */
    void addText(std::string str);
    /**
     * @brief Add an success text to the build-in terminal
     * @param str String containing the text to be added.
     */
    void addSuccess(std::string str);
    /**
     * @brief Add a warning text to the build-in terminal
     * @param str String containing the text to be added.
     */
    void addWarning(std::string str);
    /**
     * @brief Add an error text to the build-in terminal
     * @param str String containing the text to be added.
     */
    void addError(std::string str);
    /**
     * @brief Set the text in the build-in terminal
     * @param str String containing the text to be set.
     */
    void setText(std::string str);
    /**
     * @brief Give the current text of the build-in terminal
     * @return Gives a QString text of the build-in terminal.
     */
    std::string getText();

protected:
    /**
     * @brief Check if the files are saved when the window is closed.
     * @param[out] event The closing event.
     */
    void closeEvent(QCloseEvent *event);
    
private:
    Ui::GUIController *ui; /**< TODO */
    /**
     * @brief
     *
     */
    void readSettings();
    /**
     * @brief
     *
     */
    void writeSettings();
    /**
     * @brief Saves a modified file from an text editor.
     * @param[in] editor The editor that should be saved.
     * @return Gives true when the file is saved else false.
     */
    bool checkSave(QTextEdit *editor);
    /**
     * @brief Opens the file of a specific filename and text editor.
     * @param[in] fileName The name of the file that should be opened.
     * @param[out] editor The editor that get the file content.
     */
    void openFile(const QString &fileName, QTextEdit *editor);
    /**
     * @brief Saves the file of a specific filename and text editor.
     * @param[out] fileName The name of the file that should be saved.
     * @param[in] editor The editor that contains the content that should be saved.
     * @return bool Gives true when the file is saved else false.
     */
    bool saveFile(const QString &fileName, QTextEdit *editor);
    /**
     * @brief Displays the current model file name.
     * @param fileName The name of the current model file.
     */
    void modelSetCurrentFile(const QString &fileName);
    /**
     * @brief Open the file with the given filename.
     */
    void modelOpen(QString filename);
    /**
     * @brief
     *
     * @param fullFileName
     * @param editor
     * @return QString
     */
    QString strippedName(const QString &fullFileName, QTextEdit *editor);
    /**
     * @brief
     *
     * @param Str
     * @return std::string
     */
    std::string escapeHTML(std::string & Str);

    QString modelCurFile; /**< TODO */
    QString modelFileName; /**< TODO */

    int pc; /**< TODO */

/* This is the Controller part of the class.
 * Slots receive signals from the view, while processing these slots to a model.
 */
private slots:
    /**
     * @brief Creates a new model file in the model text editor.
     */
    void modelNew();
    /**
     * @brief Ask to define a file name for the model and open the file.
     */
    void modelOpen();
    /**
     * @brief
     *
     */
    void exampleReservoir();
    /**
     * @brief
     *
     */
    void exampleWater();
    /**
     * @brief
     *
     */
    void exampleSewage();
    /**
     * @brief
     *
     */
    void exampleOverflow();
    /**
     * @brief Saves the current model file.
     * @return bool Gives true if the model file is saved else false.
     */
    bool modelSave();
    /**
     * @brief Gives a dialog where to save the model file and saves the file.
     * @return bool Gives true if the model file is saved else false.
     */
    bool modelSaveAs();
    /**
     * @brief Places an asterix near the filename to indicate that the model file was modified.
     */
    void modelModified();
    /**
     * @brief Gives some information about the tool in a small about pop-up.
     */
    void about();
    /**
     * @brief Opens the googlecode project website.
     */
    void openProjectWebsite();
    /**
     * @brief Generates an STD.
     */
    void generateSTD();
    /**
     * @brief Generates an probability function for a given place.
     */
    void generateProbFunc();
    /**
     * @brief
     *
     */
    void modelCheck();
//    void tempModelCheck();
};
#endif // GUICONTROLLER_H
