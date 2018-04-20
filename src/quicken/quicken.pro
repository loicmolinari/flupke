TARGET = Quicken
QT = core-private gui-private quick-private

contains(QT_CONFIG, opengles2) {
    CONFIG += egl
    DEFINES += MESA_EGL_NO_X11_HEADERS
}

HEADERS += \
    $$PWD/applicationmonitor.h \
    $$PWD/applicationmonitor_p.h \
    $$PWD/bitmaptext_p.h \
    $$PWD/bitmaptextfont_p.h \
    $$PWD/gputimer_p.h \
    $$PWD/logger.h \
    $$PWD/logger_p.h \
    $$PWD/magnifier_p.h \
    $$PWD/magnifiernode_p.h \
    $$PWD/metrics.h \
    $$PWD/metrics_p.h \
    $$PWD/overlay_p.h \
    $$PWD/quickenglobal.h \
    $$PWD/quickenglobal_p.h

SOURCES += \
    $$PWD/applicationmonitor.cpp \
    $$PWD/bitmaptext.cpp \
    $$PWD/gputimer.cpp \
    $$PWD/logger.cpp \
    $$PWD/magnifier.cpp \
    $$PWD/magnifiernode.cpp \
    $$PWD/metrics.cpp \
    $$PWD/overlay.cpp \
    $$PWD/quickenglobal.cpp

RESOURCES += \
    $$PWD/resources.qrc

OTHER_FILES += \
    $$PWD/shaders/magnifier.frag \
    $$PWD/shaders/magnifier.vert \
    $$PWD/shaders/magnifier_core.frag \
    $$PWD/shaders/magnifier_core.vert \
    $$PWD/shaders/magnifieropaque.frag \
    $$PWD/shaders/magnifieropaque_core.frag

load(quicken_qt_module)
