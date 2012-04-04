# Qmake project file
#
# run `qmake` to generate Makefile

QT += sql

# On OSX, do not place the executable in a bundle
CONFIG -= app_bundle

# CONFIG += debug

#QMAKE_CLEAN += Makefile
QMAKE_CLEAN += $(TARGET)

HEADERS = \
  main_dialog.h \
  map_scene.h

SOURCES = \
  main.cpp \
  main_dialog.cpp \
  map_scene.cpp \

FORMS = \
  main_dialog.ui
