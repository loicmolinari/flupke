TEMPLATE = subdirs

# Libraries

perf_lib.subdir = QuickenPerf
perf_lib.target = QuickenPerf-lib
SUBDIRS += perf_lib

# QML modules

# perf_module.subdir = imports/Perf
# perf_module.target = perf-module
# perf_module.depends = perf-lib
# SUBDIRS += perf_module
