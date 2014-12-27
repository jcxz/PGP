#
# @author: Matus Fedorko
#

QT += core
QT += gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PGP
#CONFIG += console
#CONFIG -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -pedantic -g

# aby hlavickove subory nasiel aj Qt Designer
INCLUDEPATH += src


HEADERS += \
    src/ogl.h \
    src/logger.h \
    src/trackball.h \
    src/volume_renderer.h \
    src/debug_volume_renderer.h \
    src/texture_volume_renderer.h \
    src/volume_viewer.h \
    src/main_window.h \
    src/volume_data.h \
    src/transfer_function_editor.h \
    src/transfer_function.h \
    src/thirdparty/codebase.h \
    src/thirdparty/ddsbase.h

SOURCES += \
    src/logger.cpp \
    src/trackball.cpp \
    src/volume_renderer.cpp \
    src/debug_volume_renderer.cpp \
    src/texture_volume_renderer.cpp \
    src/volume_viewer.cpp \
    src/main_window.cpp \
    src/main.cpp \
    src/volume_data.cpp \
    src/transfer_function_editor.cpp \
    src/transfer_function.cpp \
    src/thirdparty/ddsbase.cpp

FORMS += \
    src/main_window.ui

RESOURCES += \
    PGP_shaders.qrc \
    PGP_data.qrc

OTHER_FILES +=
