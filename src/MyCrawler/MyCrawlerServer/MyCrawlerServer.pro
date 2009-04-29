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
HEADERS += MCServerApplication.h \
    ../Components/Components.h
SOURCES += main.cpp \
    MCServerApplication.cpp
OTHER_FILES += ../TODO.txt
include(../Components/Debug.pri)
include(../Components/Core.pri)
include(../Components/Config.pri)
include(Network.pri)
include(GUI.pri)

