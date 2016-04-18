#-------------------------------------------------
#
# Project created by QtCreator 2016-04-13T18:48:21
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MEX_PerfMon
TEMPLATE = app


SOURCES += main.cpp\
        mex_perfmon.cpp \
    mex_perfmonserver.cpp

HEADERS  += mex_perfmon.h \
    mex_perfmonserver.h

FORMS    += mex_perfmon.ui
