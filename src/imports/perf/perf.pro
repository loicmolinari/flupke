CXX_MODULE = qml
TARGET = quickenperfplugin
TARGETPATH = Quicken/Perf
IMPORT_VERSION = 0.1

SOURCES += plugin.cpp

QT += quicken-private qml-private

load(quicken_qml_plugin)
