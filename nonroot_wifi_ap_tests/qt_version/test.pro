QT += network
QT -= gui

CONFIG -= app_bundle
CONFIG += mobility

MOBILITY += systeminfo

QMAKE_CLEAN += $(TARGET)

SOURCES = main.cpp
