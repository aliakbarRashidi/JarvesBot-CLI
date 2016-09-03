QT += core network
QT -= gui

CONFIG += c++11

TARGET = JarvesBot_CLI
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    longpoll.cpp \
    Json/json.cpp

HEADERS += \
    longpoll.h \
    Json/json.h \
    network.h
