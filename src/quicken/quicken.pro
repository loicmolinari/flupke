TARGET = Quicken
QT = core-private quick-private

contains(QT_CONFIG, opengles2) {
    CONFIG += egl
    DEFINES += MESA_EGL_NO_X11_HEADERS
}

include(items/items.pri)
include(perf/perf.pri)

HEADERS += \
    $$PWD/quickenglobal.h \
    $$PWD/quickenglobal_p.h

SOURCES += \
    $$PWD/quickenglobal.cpp

load(quicken_qt_module)
