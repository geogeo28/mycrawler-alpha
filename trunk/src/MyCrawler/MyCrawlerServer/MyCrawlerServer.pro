QT += network
TARGET = MyCrawlerServer
TEMPLATE = app
CONFIG += qt \
    thread \
    exception

# Debug build options
CONFIG(debug, debug|release):CONFIG += console # Enable a read-only console window (i.e. for printf etc.)
INCLUDEPATH += .
INCLUDEPATH += ../Components/

PRECOMPILED_HEADER = Components.h

HEADERS += ../Components/Config.h \
    MCServer.h \
    MCClientThreadManager.h \
    MCClientThread.h \
    MCClientPeer.h \
    MCServerApplication.h \
    ../Components/Components.h
SOURCES += main.cpp \
    MCServer.cpp \
    MCClientThreadManager.cpp \
    MCClientThread.cpp \
    MCClientPeer.cpp \
    MCServerApplication.cpp
OTHER_FILES += ../TODO.txt
include(../Components/Debug.pri)
include(../Components/Core.pri)
