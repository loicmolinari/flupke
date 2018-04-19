BUILD_PATHS_CONTENT = \
  "SRC_DIR=\"$$PWD\"" \
  "BUILD_DIR=\"$$OUT_PWD\""
write_file($$PWD/.build_paths.inc, BUILD_PATHS_CONTENT)

requires(qtHaveModule(quick))
load(qt_parts)

SUBDIRS += src tools

OTHER_FILES += \
    $$PWD/.qmake.conf \
    $$PWD/features/quicken_common.prf \
    $$PWD/features/quicken_qt_module.prf
