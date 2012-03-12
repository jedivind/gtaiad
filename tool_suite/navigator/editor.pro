# Qmake project file
#
# run `qmake` to generate Makefile

QT += sql

# On OSX, do not place the executable in a bundle
CONFIG -= app_bundle

# CONFIG += debug

#QMAKE_CLEAN += Makefile
QMAKE_CLEAN += $(TARGET)

SOURCES = \
  main.cpp

FORMS = \
  main_dialog.ui
