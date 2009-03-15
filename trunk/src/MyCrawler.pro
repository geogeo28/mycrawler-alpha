#-------------------------------------------------
#
# Project created by QtCreator 2009-03-15T18:02:43
#
#-------------------------------------------------

QT       += network opengl sql xml

TARGET = MyCrawler
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

include(Network/network.pri)
include(Ogre/Ogre.pri)
