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


# TODO: не работает дебаг спрайтов
#DEFINES += DEBUG_SPRITE

SOURCES += \
            src/main.cpp \
            src/sprite.cpp \
            src/playersprite.cpp \
            src/bulletsprite.cpp \
            src/mapfield.cpp \
    src/mediator.cpp \
    src/menu.cpp \
    src/settingview.cpp

HEADERS += \
            src/sprite.hpp \
            src/enums.hpp \
            src/playersprite.hpp \
            src/bulletsprite.hpp \
            src/mapfield.hpp \
    src/mediator.hpp \
    src/menu.hpp \
    src/settingview.hpp

RESOURCES += \
    src/resource.qrc

FORMS += \
    src/settingview.ui


