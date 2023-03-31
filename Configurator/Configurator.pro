QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

TARGET = tvvm-configurator

RC_ICONS = config_icon.ico

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../logger ui tree_model models

SOURCES += \
    ../logger/loggable.cpp \
    ../logger/logger.cpp \
    configmanager.cpp \
    main.cpp \
    mainwindow.cpp \
    models/configsproxymodel.cpp \
    tree_model/configfoldertreeitem.cpp \
    tree_model/configtreeitem.cpp \
    tree_model/servicetreeitem.cpp \
    tree_model/sourcefoldertreeitem.cpp \
    tree_model/sourcetreeitem.cpp \
    tree_model/treeitem.cpp \
    tree_model/treemodel.cpp \
    tree_model/viewerwindowfoldertreeitem.cpp \
    tree_model/viewerwindowtreeitem.cpp \
    ui/checkboxdelegate.cpp \
    ui/configsform.cpp

HEADERS += \
    ../logger/loggable.h \
    ../logger/logger.h \
    configmanager.h \
    configstructs.h \
    mainwindow.h \
    models/configsproxymodel.h \
    tree_model/configfoldertreeitem.h \
    tree_model/configtreeitem.h \
    tree_model/servicetreeitem.h \
    tree_model/sourcefoldertreeitem.h \
    tree_model/sourcetreeitem.h \
    tree_model/treeitem.h \
    tree_model/treemodel.h \
    tree_model/viewerwindowfoldertreeitem.h \
    tree_model/viewerwindowtreeitem.h \
    ui/checkboxdelegate.h \
    ui/configsform.h

FORMS += \
    mainwindow.ui \
    ui/configsform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$PWD/../ffmpeg-5.1.2-full_build-shared/include
#INCLUDEPATH += ../ffmpeg-5.1.2-full_build-shared/include
LIBS += $$PWD/../ffmpeg-5.1.2-full_build-shared/lib/avcodec.lib \
        $$PWD/../ffmpeg-5.1.2-full_build-shared/lib/avdevice.lib \
        $$PWD/../ffmpeg-5.1.2-full_build-shared/lib/avfilter.lib \
        $$PWD/../ffmpeg-5.1.2-full_build-shared/lib/avformat.lib \
        $$PWD/../ffmpeg-5.1.2-full_build-shared/lib/avutil.lib \
        $$PWD/../ffmpeg-5.1.2-full_build-shared/lib/postproc.lib \
        $$PWD/../ffmpeg-5.1.2-full_build-shared/lib/swresample.lib \
        $$PWD/../ffmpeg-5.1.2-full_build-shared/lib/swscale.lib

LIBS += -L$$PWD/../ffmpeg-5.1.2-full_build-shared/lib
LIBS += -L$$PWD/../ffmpeg-5.1.2-full_build-shared/bin
LIBS += -lavcodec \
        -lavformat \
        -lavutil

#copydata.commands = mklink $$shell_path($$OUT_PWD/tvvm-config.db) $$shell_path($$PWD/../configs/tvvm-config.db)
copydata.commands = $(COPY_DIR) $$shell_path($$PWD/../configs/tvvm-config.db) $$shell_path($$OUT_PWD)
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata
