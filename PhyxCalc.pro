#-------------------------------------------------
#
# Project created by QtCreator 2011-11-07T14:50:36
#
#-------------------------------------------------
# Add files and directories to ship with the application 
# by adapting the examples below.
# file1.source = myfile
# dir1.source = mydir
#DEPLOYMENTFOLDERS = # file1 dir1

symbian:TARGET.UID3 = 0xE4EA669C

# Smart Installer package's UID
# This UID is from the protected range 
# and therefore the package will fail to install if self-signed
# By default qmake uses the unprotected range value if unprotected UID is defined for the application
# and 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
# symbian:TARGET.CAPABILITY += NetworkServices

# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the 
# MOBILITY variable. 
# CONFIG += mobility
# MOBILITY +=


# Qt6 modules
QT += QtCore QtGui QtSvg QtWidgets QtPrintSupport

# Qt6 configuration
CONFIG += c++20


TARGET = phyxcalc
TEMPLATE = app

win32 {
    INCLUDEPATH += ../boost \
                ../qwt6/src
    LIBS += -L../qwt6/lib
}
android {
    INCLUDEPATH += ../boost \
                ../qwt6/src
}
symbian {
    INCLUDEPATH += ../boost \
                ../qwt6/src
}

linux-g++ | linux-g++-64 | linux-g++-32 {
    INCLUDEPATH += /usr/include/qwt5 \  #openSUSE
                /usr/include/qwt6 \  #openSUSE
                /usr/include/qwt \      #Fedora
                /usr/include/qwt-qt4    #ubuntu
}

LIBS += -lqwt

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
    phyxvariablemanager.cpp \
    phyxsyntaxhighlighter.cpp \
    helpdialog.cpp \
    plotwindow.cpp \
    plotdialog.cpp

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
    phyxvariablemanager.h \
    phyxsyntaxhighlighter.h \
    helpdialog.h \
    plotwindow.h \
    plotdialog.h

FORMS    += mainwindow.ui \
    exportdialog.ui \
    settingsdialog.ui \
    helpdialog.ui \
    plotwindow.ui \
    plotdialog.ui

RESOURCES += \
    images.qrc \
    settings.qrc

TRANSLATIONS = l10n/phyxcalc_de.ts \
    l10n/phyxcalc_fr.ts

OTHER_FILES += \
    settings/grammar.txt \
    settings/definitions.txt \
    settings/symbols.txt \
    settings/settings.ini \
    settings/docks.txt \
    settings/electronicFunctions.txt \
    settings/numbersDock.txt \
    settings/operatorsDock.txt \
    doc/doc_en.txt \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()

OTHER_FILES += \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-et/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-el/strings.xml \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/AndroidManifest.xml \
    android/version.xml

#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg
