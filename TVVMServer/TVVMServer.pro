QT       += core gui sql multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
TARGET = tvvm-server

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../logger media player audio ui

SOURCES += \
    ../logger/loggable.cpp \
    ../logger/logger.cpp \
    audio/audiolevelcalculator.cpp \
    audio/audiolevelmeter.cpp \
    audio/audiolevelwidget.cpp \
    audio/audiooutput.cpp \
    audio/loudnesscalculator.cpp \
    configmanager.cpp \
    media/mediaservice.cpp \
    main.cpp \
    mainwindow.cpp \
    media/mediasource.cpp \
    player/audiodecoder.cpp \
    player/audioframe.cpp \
    player/decoder.cpp \
    player/decoderworker.cpp \
    player/demuxer.cpp \
    player/ffmpegfilter.cpp \
    player/frame.cpp \
    player/videodecoder.cpp \
    player/videoframe.cpp \
    ui/mediaviewerwindow.cpp \
    ui/mediawidget.cpp \
    ui/tvservicewidget.cpp

HEADERS += \
    ../logger/loggable.h \
    ../logger/logger.h \
    audio/audiolevelcalculator.h \
    audio/audiolevelmeter.h \
    audio/audiolevelwidget.h \
    audio/audiooutput.h \
    audio/concretesamplesextractor.h \
    audio/loudnesscalculator.h \
    audio/samplesextractor.h \
    configmanager.h \
    configstructs.h \
    media/mediaservice.h \
    mainwindow.h \
    media/mediasource.h \
    player/audiodecoder.h \
    player/audioframe.h \
    player/decoder.h \
    player/decoderworker.h \
    player/demuxer.h \
    player/ffmpegfilter.h \
    player/frame.h \
    player/utils.h \
    player/videodecoder.h \
    player/videoframe.h \
    ui/mediaviewerwindow.h \
    ui/mediawidget.h \
    ui/tvservicewidget.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#INCLUDEPATH += $$PWD/../ffmpeg-5.1.2-full_build-shared/include
INCLUDEPATH += ../ffmpeg-5.1.2-full_build-shared/include
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

RESOURCES += \
    ../images.qrc

#copydata.commands = mklink $$shell_path($$OUT_PWD/tvvm-config.db) $$shell_path($$PWD/../configs/tvvm-config.db)
copydata.commands = $(COPY_DIR) $$shell_path($$PWD/../configs/tvvm-config.db) $$shell_path($$OUT_PWD)
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata
