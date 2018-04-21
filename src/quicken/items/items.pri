HEADERS += \
    $$PWD/magnifier_p.h \
    $$PWD/magnifiernode_p.h

SOURCES += \
    $$PWD/magnifier.cpp \
    $$PWD/magnifiernode.cpp

RESOURCES += \
    $$PWD/resources.qrc

OTHER_FILES += \
    $$PWD/shaders/magnifier.frag \
    $$PWD/shaders/magnifier.vert \
    $$PWD/shaders/magnifier_core.frag \
    $$PWD/shaders/magnifier_core.vert \
    $$PWD/shaders/magnifieropaque.frag \
    $$PWD/shaders/magnifieropaque_core.frag
