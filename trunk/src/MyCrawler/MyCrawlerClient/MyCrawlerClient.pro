QT += network
TARGET = MyCrawlerClient
TEMPLATE = app
CONFIG += qt \
    thread \
    exception

# Debug build options
CONFIG(debug, debug|release):CONFIG += console # Enable a read-only console window (i.e. for printf etc.)
INCLUDEPATH += .
INCLUDEPATH += ../Components/
INCLUDEPATH += ../Shared/
PRECOMPILED_HEADER = Components.h
HEADERS += ClientApplication.h \
    ../Components/Components.h
SOURCES += main.cpp \
    ClientApplication.cpp
OTHER_FILES += ../TODO.txt
include(../Components/Debug.pri)
include(../Components/Core.pri)
include(../Components/Config.pri)
include(../Components/Utilities.pri)
include(../Components/Widgets.pri)
include(../Shared/Shared.pri)
include(Network.pri)
include(GUI.pri)
