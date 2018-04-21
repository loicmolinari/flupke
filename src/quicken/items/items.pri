CONFIG(debug, debug|release) {
    HEADERS += \
        $$PWD/quickenboilerplate_p.h \
        $$PWD/quickenboilerplatenode_p.h

    SOURCES += \
        $$PWD/quickenboilerplate.cpp \
        $$PWD/quickenboilerplatenode.cpp

    RESOURCES += \
        $$PWD/resources.qrc

    OTHER_FILES += \
        $$PWD/shaders/boilerplate.frag \
        $$PWD/shaders/boilerplate.vert \
        $$PWD/shaders/boilerplate_core.frag \
        $$PWD/shaders/boilerplate_core.vert \
        $$PWD/shaders/boilerplateopaque.frag \
        $$PWD/shaders/boilerplateopaque_core.frag
}
