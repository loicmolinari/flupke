requires(qtHaveModule(quick))
load(qt_parts)

SUBDIRS += src

OTHER_FILES += \
    $$PWD/.qmake.conf \
    $$PWD/features/flupke_common.prf \
    $$PWD/features/flupke_qt_module.prf
