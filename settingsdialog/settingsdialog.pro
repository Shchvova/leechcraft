######################################################################
# Automatically generated by qmake (2.01a) Sun Jun 17 17:53:12 2007
######################################################################

TEMPLATE = lib
TARGET = settingsdialog
DESTDIR = .
win32:DESTDIR = ../
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += settingsdialog.h settingsiteminfo.h typehandler.h typelist.h converters.h filepickerwidget.h datatypes.h
SOURCES += settingsdialog.cpp settingsiteminfo.cpp typehandler.cpp typelist.cpp converters.cpp filepickerwidget.cpp datatypes.cpp
CONFIG += release threads

unix:themain.path=/usr/lib
unix:themain.files=libsettingsdialog.so*
win32:themain.path=../
win32:themain.files=release/settingsdialog.dll release/libsettingsdialog.a
INSTALLS += themain
