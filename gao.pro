#-------------------------------------------------
#
# Project created by QtCreator 2012-12-21T09:21:34
#
#-------------------------------------------------

QT       += core gui phonon

TARGET = gao
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    main.cpp \
    SUtil.cpp \
    SModel.cpp \
    SMelody.cpp \
    SDSP.cpp \
    SBuildModel.cpp \
    midifile.c \
    mainwindow.cpp \
    main.cpp

HEADERS  += mainwindow.hpp \
    SUtil.h \
    STypes.h \
    SModel.h \
    SMelody.h \
    SDSP.h \
    SBuildModel.h \
    midifile.h \
    mainwindow.hpp \
    SDFuzzySearch.h

FORMS    += mainwindow.ui \
    mainwindow.ui
# LIBS += "D:\sdhumming\Gao\SDFuzzySearch.lib"

OTHER_FILES += \
    SDHBuildModel.ilk \
    SDFuzzySearch.lib \
    SDFuzzySearch.dll \
    config.txt
