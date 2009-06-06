QT += network opengl
TARGET = MyCrawlerServer
TEMPLATE = app
CONFIG += qt \
    thread \
    exception

DEFINES += MYCRAWLER_SERVER

# Debug build options
CONFIG(debug, debug|release):CONFIG += console # Enable a read-only console window (i.e. for printf etc.)
INCLUDEPATH += .
INCLUDEPATH += ../Components/
INCLUDEPATH += ../Shared/
PRECOMPILED_HEADER = Components.h
HEADERS += ServerApplication.h \
    ../Components/Components.h \
    SettingsApplication.h
SOURCES += main.cpp \
    ServerApplication.cpp \
    SettingsApplication.cpp
OTHER_FILES += ../TODO.txt
include(../Components/Debug.pri)
include(../Components/Core.pri)
include(../Components/Config.pri)
include(../Components/Utilities.pri)
include(../Components/Widgets.pri)
include(../Shared/Shared.pri)
include(../QtGraph/QtGraph.pri)
include(../Graph/Graph.pri)
include(Network.pri)
include(GUI.pri)
RESOURCES += resources.qrc
