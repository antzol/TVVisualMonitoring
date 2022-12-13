QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = tvvm-configurator

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../logger ui

SOURCES += \
    ../logger/loggable.cpp \
    ../logger/logger.cpp \
    main.cpp \
    mainwindow.cpp \
    ui/checkboxdelegate.cpp \
    ui/configsgroupbox.cpp \
    ui/editservicedialog.cpp \
    ui/editsourcedialog.cpp \
    ui/editviewerwindowdialog.cpp \
    ui/servicesgroupbox.cpp \
    ui/sourcesgroupbox.cpp \
    ui/viewerwindowsgroupbox.cpp

HEADERS += \
    ../logger/loggable.h \
    ../logger/logger.h \
    configstructs.h \
    mainwindow.h \
    ui/checkboxdelegate.h \
    ui/configsgroupbox.h \
    ui/editservicedialog.h \
    ui/editsourcedialog.h \
    ui/editviewerwindowdialog.h \
    ui/servicesgroupbox.h \
    ui/sourcesgroupbox.h \
    ui/viewerwindowsgroupbox.h

FORMS += \
    mainwindow.ui \
    ui/configsgroupbox.ui \
    ui/editservicedialog.ui \
    ui/editsourcedialog.ui \
    ui/editviewerwindowdialog.ui \
    ui/servicesgroupbox.ui \
    ui/sourcesgroupbox.ui \
    ui/viewerwindowsgroupbox.ui

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
