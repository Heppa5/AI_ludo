#-------------------------------------------------
#
# Project created by QtCreator 2016-03-15T10:40:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ludo
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    game.cpp \
    ludo_player.cpp \
    ludo_player_random.cpp \

HEADERS  += dialog.h \
    game.h \
    ludo_player.h \
    positions_and_dice.h \
    ludo_player_random.h \
    individual.h \
    functions_c_edition.h

FORMS    += dialog.ui

LIBS += -lfann

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Wunused
