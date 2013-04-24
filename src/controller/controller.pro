#-------------------------------------------------
#
# Controller configuration file created by B.F. Postema
# Everything from the UI as possible input will be processed within the controller.
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

LIBS += -L../model -L../libs -lmodel -lmatheval -lopencv_core -lopencv_imgproc -lopencv_highgui
#LIBS += -lopencv_core.a
#LIBS += ../libs/libopencv_imgproc.a
#LIBS += ../libs/libopencv_highgui.a
#-lopencv_core -lopencv_imgproc -lopencv_highgui -ltbb

#Make the tool run with the actual shared library of the model.
QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN
QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/model
QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/libs
#QMAKE_LFLAGS += -Wl,-rpath-link=\\\$\$ORIGIN/libs
QMAKE_LFLAGS_RPATH=

#INCLUDEPATH += /usr/include/opencv \

#LIBS    +=  -lmatheval -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann

TARGET = ../project

SOURCES +=  main.cpp\
            GUIController.cpp

HEADERS +=  GUIController.h

FORMS       +=  ../view/GUIView.ui\
                ../view/GUIViewDesign.ui

RESOURCES   +=  ../view/GUIResources.qrc\
