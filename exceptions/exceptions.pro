######################################################################
# Automatically generated by qmake (2.01a) Tue May 29 02:14:11 2007
######################################################################

TEMPLATE = lib
TARGET = exceptions
CONFIG += release
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = .
win32:DESTDIR = ..

# Input
HEADERS += generic.h invalidparameter.h logic.h notimplemented.h outofbounds.h io.h situative.h
SOURCES += generic.cpp invalidparameter.cpp logic.cpp notimplemented.cpp io.cpp situative.cpp
QT -= network gui core


unix:themain.path=/usr/lib
unix:themain.files=libexceptions.so*
win32:themain.path=../
win32:themain.files=release/exceptions.dll release/libexceptions.a
INSTALLS += themain
