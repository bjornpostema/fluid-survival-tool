#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H

#include <QMainWindow>
#include <QString>
#include <QPlainTextEdit>

namespace Ui {
class GUIController;
}

class GUIController : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit GUIController(QWidget *parent = 0);
    ~GUIController();

    void addText(QString str);
    void setText(QString str);
    QString getText();

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
    void specSetCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName, QTextEdit *editor);

    QString modelCurFile, specCurFile;
    QString modelFileName, specFileName;

/* This is the Controller part of the class.
 * Slots receive signals from the view, while processing these slots to a model.
 */
private slots:
    void modelNew();
    void modelOpen();
    bool modelSave();
    bool modelSaveAs();
    void modelModified();
    void specNew();
    void specOpen();
    bool specSave();
    bool specSaveAs();
    void specModified();
    void about();
    void openProjectWebsite();
    void generateSTD();
    void generateProbFunc();
    void modelCheck();
};
#endif // GUICONTROLLER_H
