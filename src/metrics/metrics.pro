TARGET = FlupkeMetrics
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
    $$PWD/events.h \
    $$PWD/events_p.h \
    $$PWD/flupkemetricsglobal.h \
    $$PWD/flupkemetricsglobal_p.h \
    $$PWD/gputimer_p.h \
    $$PWD/logger.h \
    $$PWD/logger_p.h \
    $$PWD/overlay_p.h

SOURCES += \
    $$PWD/applicationmonitor.cpp \
    $$PWD/bitmaptext.cpp \
    $$PWD/events.cpp \
    $$PWD/flupkemetricsglobal.cpp \
    $$PWD/gputimer.cpp \
    $$PWD/logger.cpp \
    $$PWD/overlay.cpp

load(flupke_qt_module)
