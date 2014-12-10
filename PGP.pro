#
# @author: Matus Fedorko
#

QT += core
QT += gui

TARGET = PGP
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -pedantic -g


HEADERS += \
    src/ogl.h \
    src/texture3d.h \
    src/opengl_window.h \
    src/volume_window.h

SOURCES += \
    src/main.cpp \
    src/ogl.cpp \
    src/texture3d.cpp \
    src/opengl_window.cpp \
    src/volume_window.cpp

RESOURCES += \
    PGP_shaders.qrc \
    PGP_data.qrc

OTHER_FILES +=
