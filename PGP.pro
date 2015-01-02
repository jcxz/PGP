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
    src/volume_data_histogram.h \
    src/transfer_function_editor.h \
    src/transfer_function.h \
    src/color_picker.h \
    src/color_luminance_picker.h \
    src/thirdparty/codebase.h \
    src/thirdparty/ddsbase.h \
    src/ray_cast_volume_renderer.h \
    src/raw_file_details_dlg.h

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
    src/volume_data_histogram.cpp \
    src/transfer_function_editor.cpp \
    src/transfer_function.cpp \
    src/color_picker.cpp \
    src/color_luminance_picker.cpp \
    src/thirdparty/ddsbase.cpp \
    src/ray_cast_volume_renderer.cpp \
    src/raw_file_details_dlg.cpp

FORMS += \
    src/main_window.ui \
    src/raw_file_details_dlg.ui

RESOURCES += \
    PGP_shaders.qrc \
    PGP_data.qrc

OTHER_FILES +=
