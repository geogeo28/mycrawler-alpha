# -------------------------------------------------
# Project created by QtCreator 2009-04-28T13:53:12
# -------------------------------------------------
QT += network \
    opengl
TARGET = Graph
TEMPLATE = app
CONFIG += qt \
    thread \
    exception

# Debug build options
CONFIG(debug, debug|release):CONFIG += console # Enable a read-only console window (i.e. for printf etc.)
INCLUDEPATH += .
INCLUDEPATH += ../Components/
PRECOMPILED_HEADER = Components.h
SOURCES += main.cpp \
    mainwindow.cpp \
    mcgraphapplication.cpp
HEADERS += mainwindow.h \
    ../Components/Config.h \
    ../Components/Components.h \
    mcgraphapplication.h
FORMS += mainwindow.ui
OTHER_FILES += ../TODO.txt
include(../Components/Debug.pri)
include(../Components/Core.pri)
