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
    GraphApplication.cpp
HEADERS += ../Components/Components.h \
    GraphApplication.h

FORMS += 
OTHER_FILES += ../TODO.txt
include(../Components/Debug.pri)
include(../Components/Core.pri)
include(../Components/Config.pri)
include(Graph.pri)
include(GUI.pri)
