# -------------------------------------------------
# Project created by QtCreator 2009-04-03T22:04:04
# -------------------------------------------------
QT += network
TARGET = Server
TEMPLATE = app
SOURCES += main.cpp \
    ServerMainWindow.cpp \
    Server.cpp \
    Utilities.cpp \
    MCServer.cpp \
    MCClientPeer.cpp \
    MCException.cpp
HEADERS += ServerMainWindow.h \
    Utilities.h \
    Server.h \
    MCServer.h \
    MCClientPeer.h \
    MCException.h
FORMS += ServerMainWindow.ui
