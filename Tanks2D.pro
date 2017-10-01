#-------------------------------------------------
#
# Project created by QtCreator 2017-09-16T17:58:32
#
#-------------------------------------------------

QT       += core gui
CONFIG += console c++1z

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tanks2D
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
            src/main.cpp \
            src/widget.cpp \
            src/mygraphicsview.cpp \
            src/sprite.cpp \
            src/mygraphicsscene.cpp \
            src/playersprite.cpp \
            src/playercontroller.cpp \
            src/bulletsprite.cpp \
            src/mapfield.cpp

HEADERS += \
            src/widget.h \
            src/mygraphicsview.hpp \
            src/sprite.hpp \
            src/mygraphicsscene.hpp \
            src/enums.hpp \
            src/playersprite.hpp \
            src/playercontroller.hpp \
            src/bulletsprite.hpp \
            src/mapfield.hpp

FORMS += \
            src/widget.ui
