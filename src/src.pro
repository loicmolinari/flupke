TEMPLATE = subdirs

# Libraries

metrics_lib.subdir = QuickenMetrics
metrics_lib.target = QuickenMetrics-lib
SUBDIRS += metrics_lib

# QML modules

# metrics_module.subdir = imports/Metrics
# metrics_module.target = metrics-module
# metrics_module.depends = metrics-lib
# SUBDIRS += metrics_module
