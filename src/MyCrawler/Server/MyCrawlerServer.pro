# -------------------------------------------------
# Project created by QtCreator 2009-04-03T22:04:04
# -------------------------------------------------
QT += network
TARGET = MyCrawlerServer
TEMPLATE += app \
            subdirs

CONFIG += qt thread console

INCLUDEPATH += .
INCLUDEPATH += ../Components/

SOURCES += main.cpp \
    MCServer.cpp \
    MCClientPeer.cpp \
    MCClientThread.cpp \
    MCClientThreadManager.cpp
HEADERS += MCServer.h \
    MCClientPeer.h \
    MCClientThread.h \
    MCClientThreadManager.h
OTHER_FILES += TODO.txt


SOURCES += ../Components/Debug/Logger.cpp \
           ../Components/Debug/Loggers/LoggerFile.cpp \
           ../Components/Exceptions/MCException.cpp

SUBDIRS += ../Components
