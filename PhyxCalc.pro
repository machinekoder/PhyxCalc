#-------------------------------------------------
#
# Project created by QtCreator 2011-11-07T14:50:36
#
#-------------------------------------------------

QT       += core gui

TARGET = phyxcalc
TEMPLATE = app

win32:INCLUDEPATH += ../boost

SOURCES += main.cpp\
        mainwindow.cpp \
        lineparser.cpp \
        unitloader.cpp \
        exportdialog.cpp \
        settingsdialog.cpp \
        qearleyparser.cpp \
        phyxcalculator.cpp \
    qhidingtabwidget.cpp \
    phyxunit.cpp \
    phyxvariable.cpp \
    phyxunitsystem.cpp \
    phyxcompoundunit.cpp \
    phyxtesting.cpp \
    phyxvariablemanager.cpp \
    phyxsyntaxhighlighter.cpp

HEADERS  += mainwindow.h \
            lineparser.h \
            unitloader.h \
            exportdialog.h \
            settingsdialog.h \
            global.h \
            qearleyparser.h \
            phyxcalculator.h \
    qhidingtabwidget.h \
    phyxunit.h \
    phyxvariable.h \
    phyxunitsystem.h \
    phyxcompoundunit.h \
    phyxtesting.h \
    phyxvariablemanager.h \
    phyxsyntaxhighlighter.h

FORMS    += mainwindow.ui \
    exportdialog.ui \
    settingsdialog.ui

RESOURCES += \
    images.qrc \
    settings.qrc

OTHER_FILES += \
    settings/grammar.txt \
    settings/definitions.txt \
    settings/symbols.txt \
    settings/settings.ini \
    settings/docks.txt \
    settings/electronicFunctions.txt \
    settings/functionsDock.txt \
    settings/numbersDock.txt \
    settings/operatorsDock.txt
