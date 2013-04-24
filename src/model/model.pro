#-------------------------------------------------
#
# Model configuration file created by B.F. Postema
# The model contains all c++ data structures and algorithms to calculate with HPnG
#
#-------------------------------------------------

! include( ../common.pri ) {
    error( Couldn't find the common.pri file! )
}

#INCLUDEPATH += /usr/include/opencv \
INCLUDEPATH += ../includes/opencv \
               ../includes/opencv2 \
               ../includes \

#Make the tool run with the actual shared library of the model.
#QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN
#QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/libs
#QMAKE_LFLAGS_RPATH=
#QMAKE_RPATHDIR += \\\$\$ORIGIN/libs
#QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/libs
#QMAKE_LFLAGS_RPATH=
#DEPENDPATH += . ../libs
#-L../libs -lmatheval
#-lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann
#LIBS    += -L../libs -lopencv_core -lopencv_imgproc -lopencv_highgui
#LIBS    += -L../libs/


SOURCES +=  DFPN2.cpp\
            Formula.cpp\
            GeometryHelper.cpp\
            IntervalSet.cpp\
            Line.cpp\
            ModelChecker.cpp\
            Polygon.cpp\
            TimedDiagram.cpp\
            Region.cpp\
            Calculator.cpp\


HEADERS +=  DFPN2.h\
            Event.h\
            Formula.h\
            GeometryHelper.h\
            IntervalSet.h\
            Line.h\
            ModelChecker.h\
            Polygon.h\
            TimedDiagram.h\
            Region.h\
            Calculator.h\
