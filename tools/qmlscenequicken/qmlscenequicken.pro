TEMPLATE = app
TARGET = qmlscene-quicken
QT += qml core-private gui-private quick quick-private
qtHaveModule(widgets): QT += widgets

CONFIG += no_import_scan c++11
SOURCES += main.cpp
DEFINES += QML_RUNTIME_TESTING
!contains(QT_CONFIG, no-qml-debug): DEFINES += QT_QML_DEBUG_NO_WARNING
INCLUDEPATH += $${OUT_PWD}/../../include
LIBS += -L$${OUT_PWD}/../../lib -lQuicken
QMAKE_TARGET_DESCRIPTION = QML Scene for Quicken

load(qt_tool)
