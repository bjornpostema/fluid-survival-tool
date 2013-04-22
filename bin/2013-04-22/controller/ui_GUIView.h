/********************************************************************************
** Form generated from reading UI file 'GUIView.ui'
**
** Created: Mon Apr 22 13:20:45 2013
**      by: Qt User Interface Compiler version 4.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUIVIEW_H
#define UI_GUIVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GUIController
{
public:
    QAction *actionExit;
    QAction *newModel;
    QAction *newSpec;
    QAction *openModel;
    QAction *openSpec;
    QAction *actionRun;
    QAction *actionInfo;
    QAction *actionProject_website;
    QAction *saveModel;
    QAction *saveSpec;
    QAction *saveAsModel;
    QAction *saveAsSpec;
    QAction *actionTest;
    QAction *actionWater_Treatment_Facility;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    QWidget *ModelTab;
    QGridLayout *gridLayout_4;
    QLabel *modelFileName;
    QTextEdit *modelEditor;
    QWidget *SpecTab;
    QGridLayout *gridLayout_3;
    QTextEdit *specEditor;
    QLabel *specFileName;
    QWidget *MCTab;
    QGridLayout *gridLayout_2;
    QLabel *placeLabel;
    QPushButton *generateSTD;
    QPushButton *generateProbFunc;
    QLineEdit *placeEdit;
    QTextEdit *outputEditor;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuNew;
    QMenu *menuOpen;
    QMenu *menu_Save;
    QMenu *menuSave_As;
    QMenu *menuExamples;
    QMenu *menuHelp;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *GUIController)
    {
        if (GUIController->objectName().isEmpty())
            GUIController->setObjectName(QString::fromUtf8("GUIController"));
        GUIController->setWindowModality(Qt::WindowModal);
        GUIController->setEnabled(true);
        GUIController->resize(892, 558);
        QFont font;
        font.setFamily(QString::fromUtf8("Ubuntu Mono"));
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(50);
        GUIController->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resources/resources/logo.png"), QSize(), QIcon::Normal, QIcon::Off);
        GUIController->setWindowIcon(icon);
        GUIController->setAutoFillBackground(false);
        GUIController->setStyleSheet(QString::fromUtf8(""));
        GUIController->setDockOptions(QMainWindow::AllowTabbedDocks|QMainWindow::AnimatedDocks);
        GUIController->setUnifiedTitleAndToolBarOnMac(true);
        actionExit = new QAction(GUIController);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        newModel = new QAction(GUIController);
        newModel->setObjectName(QString::fromUtf8("newModel"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Ubuntu Mono"));
        font1.setPointSize(12);
        newModel->setFont(font1);
        newSpec = new QAction(GUIController);
        newSpec->setObjectName(QString::fromUtf8("newSpec"));
        newSpec->setFont(font1);
        openModel = new QAction(GUIController);
        openModel->setObjectName(QString::fromUtf8("openModel"));
        openModel->setFont(font1);
        openSpec = new QAction(GUIController);
        openSpec->setObjectName(QString::fromUtf8("openSpec"));
        openSpec->setFont(font1);
        actionRun = new QAction(GUIController);
        actionRun->setObjectName(QString::fromUtf8("actionRun"));
        actionInfo = new QAction(GUIController);
        actionInfo->setObjectName(QString::fromUtf8("actionInfo"));
        actionProject_website = new QAction(GUIController);
        actionProject_website->setObjectName(QString::fromUtf8("actionProject_website"));
        saveModel = new QAction(GUIController);
        saveModel->setObjectName(QString::fromUtf8("saveModel"));
        saveModel->setFont(font1);
        saveSpec = new QAction(GUIController);
        saveSpec->setObjectName(QString::fromUtf8("saveSpec"));
        saveSpec->setFont(font1);
        saveAsModel = new QAction(GUIController);
        saveAsModel->setObjectName(QString::fromUtf8("saveAsModel"));
        saveAsModel->setFont(font1);
        saveAsSpec = new QAction(GUIController);
        saveAsSpec->setObjectName(QString::fromUtf8("saveAsSpec"));
        saveAsSpec->setFont(font1);
        actionTest = new QAction(GUIController);
        actionTest->setObjectName(QString::fromUtf8("actionTest"));
        actionWater_Treatment_Facility = new QAction(GUIController);
        actionWater_Treatment_Facility->setObjectName(QString::fromUtf8("actionWater_Treatment_Facility"));
        actionWater_Treatment_Facility->setFont(font1);
        centralWidget = new QWidget(GUIController);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tabWidget->setFont(font1);
        tabWidget->setLayoutDirection(Qt::LeftToRight);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setDocumentMode(true);
        tabWidget->setMovable(false);
        ModelTab = new QWidget();
        ModelTab->setObjectName(QString::fromUtf8("ModelTab"));
        gridLayout_4 = new QGridLayout(ModelTab);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        modelFileName = new QLabel(ModelTab);
        modelFileName->setObjectName(QString::fromUtf8("modelFileName"));

        gridLayout_4->addWidget(modelFileName, 0, 0, 1, 1);

        modelEditor = new QTextEdit(ModelTab);
        modelEditor->setObjectName(QString::fromUtf8("modelEditor"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(modelEditor->sizePolicy().hasHeightForWidth());
        modelEditor->setSizePolicy(sizePolicy1);
        QFont font2;
        font2.setFamily(QString::fromUtf8("Ubuntu Mono"));
        font2.setPointSize(12);
        font2.setBold(false);
        font2.setItalic(false);
        font2.setWeight(50);
        modelEditor->setFont(font2);
        modelEditor->setFrameShape(QFrame::WinPanel);
        modelEditor->setFrameShadow(QFrame::Sunken);
        modelEditor->setLineWidth(1);

        gridLayout_4->addWidget(modelEditor, 1, 0, 1, 1);

        tabWidget->addTab(ModelTab, QString());
        SpecTab = new QWidget();
        SpecTab->setObjectName(QString::fromUtf8("SpecTab"));
        gridLayout_3 = new QGridLayout(SpecTab);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        specEditor = new QTextEdit(SpecTab);
        specEditor->setObjectName(QString::fromUtf8("specEditor"));
        sizePolicy1.setHeightForWidth(specEditor->sizePolicy().hasHeightForWidth());
        specEditor->setSizePolicy(sizePolicy1);
        QFont font3;
        font3.setPointSize(12);
        specEditor->setFont(font3);

        gridLayout_3->addWidget(specEditor, 1, 0, 1, 1);

        specFileName = new QLabel(SpecTab);
        specFileName->setObjectName(QString::fromUtf8("specFileName"));

        gridLayout_3->addWidget(specFileName, 0, 0, 1, 1);

        tabWidget->addTab(SpecTab, QString());
        MCTab = new QWidget();
        MCTab->setObjectName(QString::fromUtf8("MCTab"));
        MCTab->setEnabled(true);
        gridLayout_2 = new QGridLayout(MCTab);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        placeLabel = new QLabel(MCTab);
        placeLabel->setObjectName(QString::fromUtf8("placeLabel"));
        placeLabel->setFont(font3);

        gridLayout_2->addWidget(placeLabel, 0, 0, 1, 1);

        generateSTD = new QPushButton(MCTab);
        generateSTD->setObjectName(QString::fromUtf8("generateSTD"));
        generateSTD->setEnabled(true);
        generateSTD->setFont(font2);

        gridLayout_2->addWidget(generateSTD, 2, 1, 1, 1);

        generateProbFunc = new QPushButton(MCTab);
        generateProbFunc->setObjectName(QString::fromUtf8("generateProbFunc"));
        generateProbFunc->setEnabled(true);
        generateProbFunc->setFont(font3);

        gridLayout_2->addWidget(generateProbFunc, 1, 1, 1, 1);

        placeEdit = new QLineEdit(MCTab);
        placeEdit->setObjectName(QString::fromUtf8("placeEdit"));

        gridLayout_2->addWidget(placeEdit, 0, 1, 1, 1);

        outputEditor = new QTextEdit(MCTab);
        outputEditor->setObjectName(QString::fromUtf8("outputEditor"));
        outputEditor->setEnabled(true);
        outputEditor->setFont(font2);
        outputEditor->setFrameShape(QFrame::WinPanel);
        outputEditor->setFrameShadow(QFrame::Sunken);

        gridLayout_2->addWidget(outputEditor, 3, 0, 1, 2);

        tabWidget->addTab(MCTab, QString());

        gridLayout->addWidget(tabWidget, 0, 0, 1, 1);

        GUIController->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(GUIController);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 892, 23));
        QFont font4;
        font4.setFamily(QString::fromUtf8("Ubuntu Mono"));
        font4.setPointSize(12);
        font4.setBold(true);
        font4.setWeight(75);
        menuBar->setFont(font4);
        menuBar->setLayoutDirection(Qt::LeftToRight);
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        QFont font5;
        font5.setFamily(QString::fromUtf8("Ubuntu Mono"));
        font5.setPointSize(12);
        font5.setBold(false);
        font5.setWeight(50);
        menuFile->setFont(font5);
        menuNew = new QMenu(menuFile);
        menuNew->setObjectName(QString::fromUtf8("menuNew"));
        menuNew->setFont(font1);
        menuOpen = new QMenu(menuFile);
        menuOpen->setObjectName(QString::fromUtf8("menuOpen"));
        menuOpen->setFont(font1);
        menu_Save = new QMenu(menuFile);
        menu_Save->setObjectName(QString::fromUtf8("menu_Save"));
        QFont font6;
        font6.setFamily(QString::fromUtf8("Ubuntu Mono"));
        menu_Save->setFont(font6);
        menuSave_As = new QMenu(menuFile);
        menuSave_As->setObjectName(QString::fromUtf8("menuSave_As"));
        menuSave_As->setFont(font6);
        menuExamples = new QMenu(menuFile);
        menuExamples->setObjectName(QString::fromUtf8("menuExamples"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuHelp->setFont(font1);
        GUIController->setMenuBar(menuBar);
        statusBar = new QStatusBar(GUIController);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        GUIController->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(menuNew->menuAction());
        menuFile->addAction(menuOpen->menuAction());
        menuFile->addAction(menuExamples->menuAction());
        menuFile->addSeparator();
        menuFile->addAction(menu_Save->menuAction());
        menuFile->addAction(menuSave_As->menuAction());
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuNew->addAction(newModel);
        menuNew->addAction(newSpec);
        menuOpen->addAction(openModel);
        menuOpen->addAction(openSpec);
        menu_Save->addAction(saveModel);
        menu_Save->addAction(saveSpec);
        menuSave_As->addAction(saveAsModel);
        menuSave_As->addAction(saveAsSpec);
        menuExamples->addAction(actionWater_Treatment_Facility);
        menuHelp->addAction(actionInfo);
        menuHelp->addAction(actionProject_website);

        retranslateUi(GUIController);
        QObject::connect(openModel, SIGNAL(triggered()), GUIController, SLOT(modelOpen()));
        QObject::connect(openSpec, SIGNAL(triggered()), GUIController, SLOT(specOpen()));
        QObject::connect(newModel, SIGNAL(triggered()), GUIController, SLOT(modelNew()));
        QObject::connect(newSpec, SIGNAL(triggered()), GUIController, SLOT(specNew()));
        QObject::connect(actionExit, SIGNAL(triggered()), GUIController, SLOT(close()));
        QObject::connect(actionInfo, SIGNAL(triggered()), GUIController, SLOT(about()));
        QObject::connect(saveModel, SIGNAL(triggered()), GUIController, SLOT(modelSave()));
        QObject::connect(saveSpec, SIGNAL(triggered()), GUIController, SLOT(specSave()));
        QObject::connect(saveAsModel, SIGNAL(triggered()), GUIController, SLOT(modelSaveAs()));
        QObject::connect(saveAsSpec, SIGNAL(triggered()), GUIController, SLOT(specSaveAs()));
        QObject::connect(actionProject_website, SIGNAL(triggered()), GUIController, SLOT(openProjectWebsite()));
        QObject::connect(modelEditor, SIGNAL(textChanged()), GUIController, SLOT(modelModified()));
        QObject::connect(specEditor, SIGNAL(textChanged()), GUIController, SLOT(specModified()));
        QObject::connect(generateSTD, SIGNAL(clicked()), GUIController, SLOT(generateSTD()));
        QObject::connect(generateProbFunc, SIGNAL(clicked()), GUIController, SLOT(generateProbFunc()));

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(GUIController);
    } // setupUi

    void retranslateUi(QMainWindow *GUIController)
    {
        GUIController->setWindowTitle(QApplication::translate("GUIController", "FST", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("GUIController", "E&xit", 0, QApplication::UnicodeUTF8));
        newModel->setText(QApplication::translate("GUIController", "&HPnG Model", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        newModel->setStatusTip(QApplication::translate("GUIController", "Create a new HPnG model file in the editor", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        newModel->setShortcut(QApplication::translate("GUIController", "Ctrl+N", 0, QApplication::UnicodeUTF8));
        newSpec->setText(QApplication::translate("GUIController", "&STL Specification", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        newSpec->setStatusTip(QApplication::translate("GUIController", "Create a new STL specification file in the editor", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        openModel->setText(QApplication::translate("GUIController", "&HPnG Model", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        openModel->setStatusTip(QApplication::translate("GUIController", "Open a HPnG model file in the editor", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        openModel->setShortcut(QApplication::translate("GUIController", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        openSpec->setText(QApplication::translate("GUIController", "&STL Specification", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        openSpec->setStatusTip(QApplication::translate("GUIController", "Open a STL specification file in the editor", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionRun->setText(QApplication::translate("GUIController", "Run...", 0, QApplication::UnicodeUTF8));
        actionInfo->setText(QApplication::translate("GUIController", "&About", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionInfo->setStatusTip(QApplication::translate("GUIController", "Show information the tool", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionProject_website->setText(QApplication::translate("GUIController", "&Project Website", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionProject_website->setStatusTip(QApplication::translate("GUIController", "Go to the project website", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        saveModel->setText(QApplication::translate("GUIController", "&HPnG Model", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        saveModel->setStatusTip(QApplication::translate("GUIController", "Save the HPnG model file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        saveModel->setShortcut(QApplication::translate("GUIController", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        saveSpec->setText(QApplication::translate("GUIController", "&STL Specification", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        saveSpec->setStatusTip(QApplication::translate("GUIController", "Save the STL specification file", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        saveAsModel->setText(QApplication::translate("GUIController", "&HPnG Model", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        saveAsModel->setStatusTip(QApplication::translate("GUIController", "Save a HPnG model file in a specific location", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        saveAsSpec->setText(QApplication::translate("GUIController", "&STL Specification", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        saveAsSpec->setStatusTip(QApplication::translate("GUIController", "Save a STL specification file in a specific location", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionTest->setText(QApplication::translate("GUIController", "Test", 0, QApplication::UnicodeUTF8));
        actionWater_Treatment_Facility->setText(QApplication::translate("GUIController", "Water Treatment Facility", 0, QApplication::UnicodeUTF8));
        modelFileName->setText(QApplication::translate("GUIController", "untitled.hpng", 0, QApplication::UnicodeUTF8));
        modelEditor->setHtml(QApplication::translate("GUIController", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu Mono'; font-size:12pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#####################</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># DFPN model file   #</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#####################</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; "
                        "text-indent:0px;\">#### Number of places</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">11</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># List of places</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># Syntax: type id discMarking fluidLevel fluidBound</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># Types:</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px"
                        ";\"># DISCRETE   0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># FLUID      1</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">0 Input1On   1 0 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">0 Filt1On  1 0 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">0 Stor1On  1 0 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">0 Off      0 0 0 </p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">0 time21to6   1"
                        " 0 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">0 time6to21   0 0 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">1 soft1      0 20 20</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">1 filt1      0 0 2</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">1 filt2      0 0 2</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">1 stor2      0 8 8</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">0 failonce   1 0 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
                        "#</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#### Number of transitions</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">10</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># List of transitions</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># Syntax: type id discFiringTime weight priority fluidFlowRate genFireRate(EXP)</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; tex"
                        "t-indent:0px;\"># Types:</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># DETERMINISTIC   0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># IMMEDIATE       1</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># FLUID           2</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># GENERAL         3</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">2 input1  0 0"
                        " 0 2.0 na</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">2 pump1a  0 0 0 10.0 na</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">2 pump1b  0 0 0 10.0 na</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">2 pump1c  0 0 0 1.7 na</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">2 demand21to6  0 0 0 1.0 na</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">2 demand6to21  0 0 0 2.0 na</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">0 failure  3 1 0 na</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-"
                        "block-indent:0; text-indent:0px;\">3 repair   1 1 0 0 gen{(1 - exp(-s/10))}</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">0 at6  9 1 0 0 na</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">0 at21 15 1 0 0 na</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#### Number of arcs</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">23</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; t"
                        "ext-indent:0px;\"># List of arcs</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># Syntax: type id fromId toId weight share priority</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># Types:</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># DISCRETE_INPUT  0  from a place to a transition</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># DISCRETE_OUTPUT 1  from a transition to a place</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># FLUID_INPUT     2</p>\n"
"<p style=\" margin-top:0px;"
                        " margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># FLUID_OUTPUT    3   from a transition to a fluid place</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># INHIBITOR       4</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># TEST            5   from place to transition</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"># Input,test,inhibitor: Place -&gt; Transition,  Output: Transition -&gt; Place</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin"
                        "-right:0px; -qt-block-indent:0; text-indent:0px;\">3 pipe1a input1 soft1  1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">2 pipe1b soft1  pump1a 1 1 0 </p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">3 pipe1c pump1a filt1  1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">2 pipe1d filt1  pump1b 1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">3 pipe1e pump1b filt2  1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">2 pipe1f filt2  pump1c 1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">3 pipe1g pump1c stor2  1 "
                        "1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">2 out21  stor2 demand21to6 1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">2 out6   stor2 demand6to21 1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">5 teIn1  Input1On input1 1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">5 teFi1  Filt1On  pump1a 1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">5 teSt1  Stor1On  pump1b 1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">0 in6    time21to6   at6 1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; ma"
                        "rgin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">0 in21   time6to21 at21 1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">1 out21   at6 time6to21 1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">1 out6    at21  time21to6  1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">5 test21to6 time21to6 demand21to6 1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">5 test6to21 time6to21 demand6to21 1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">0 inBreak  Input1On failure 1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-i"
                        "ndent:0; text-indent:0px;\">1 outBreak failure Off 1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">0 inRepair Off repair  1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">1 outRepair repair Input1On 1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">0 failoncetest failonce failure 1 1 0</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#</p></body></html>", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(ModelTab), QApplication::translate("GUIController", "HPnG Model", 0, QApplication::UnicodeUTF8));
        specEditor->setHtml(QApplication::translate("GUIController", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu Mono'; font-size:12pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">P&gt;0.2(AP U AP)</p></body></html>", 0, QApplication::UnicodeUTF8));
        specFileName->setText(QApplication::translate("GUIController", "untitled.stl", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(SpecTab), QApplication::translate("GUIController", "STL Specification", 0, QApplication::UnicodeUTF8));
        placeLabel->setText(QApplication::translate("GUIController", "Place", 0, QApplication::UnicodeUTF8));
        generateSTD->setText(QApplication::translate("GUIController", "Generate STD t-s-plot (requires: HPnG model)", 0, QApplication::UnicodeUTF8));
        generateProbFunc->setText(QApplication::translate("GUIController", "Generate Pr-t plot (requires: HPnG model + Place)", 0, QApplication::UnicodeUTF8));
        outputEditor->setHtml(QApplication::translate("GUIController", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu Mono'; font-size:12pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Model Checker Output:</p></body></html>", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(MCTab), QApplication::translate("GUIController", "Model Checker", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("GUIController", "&File", 0, QApplication::UnicodeUTF8));
        menuNew->setTitle(QApplication::translate("GUIController", "&New", 0, QApplication::UnicodeUTF8));
        menuOpen->setTitle(QApplication::translate("GUIController", "&Open", 0, QApplication::UnicodeUTF8));
        menu_Save->setTitle(QApplication::translate("GUIController", "&Save", 0, QApplication::UnicodeUTF8));
        menuSave_As->setTitle(QApplication::translate("GUIController", "Save &As", 0, QApplication::UnicodeUTF8));
        menuExamples->setTitle(QApplication::translate("GUIController", "Examples", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("GUIController", "&Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class GUIController: public Ui_GUIController {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUIVIEW_H
