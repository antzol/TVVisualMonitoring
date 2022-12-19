#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QComboBox>
#include <QGroupBox>
#include <QToolButton>
#include <QPushButton>

#include <QThread>

#include "demuxer.h"

//---------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qRegisterMetaType<QtMsgType>("SourceType");

    ui->setupUi(this);

    QVBoxLayout *lay = new QVBoxLayout();
    sourcesLayout = new QGridLayout();
    lay->addLayout(sourcesLayout);
    lay->addStretch(1);
    ui->sourcesBox->setLayout(lay);

    mediaWindowsMenu = menuBar()->addMenu(tr("Media windows"));

    configManager = ConfigManager::getInstance();

    loadActiveConfig();

    audioOutput = new AudioOutput(this);

    connect(ui->toggleVolumeButton, &QPushButton::toggled, this, &MainWindow::toggleAudioVolume);
    connect(ui->routeAudioStreamButton, &QPushButton::clicked, this, &MainWindow::routeAudioStream);

    setMinimumWidth(sizeHint().width());
    setFixedHeight(sizeHint().height());
}

//---------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}

//---------------------------------------------------------------------------------------
void MainWindow::openMosaicWindow()
{
    QAction *act = qobject_cast<QAction*>(sender());

    if (!act)
        return;

    auto it = mosaicWindowActions.find(act);
    if (it != mosaicWindowActions.end())
    {
        QString msg = QString("Open mosaic window \"%1\"").arg(it->second->windowTitle());
        loggable.logMessage(objectName(), QtDebugMsg, msg);
        it->second->show();
        it->second->raise();
    }
}

//---------------------------------------------------------------------------------------
void MainWindow::setStartButtonLocked(bool locked)
{
    MediaSource *source = qobject_cast<MediaSource*>(sender());
    if (!source)
        return;

    int sourceId = source->getId();

    QString msg = QString("Update start button state for source (id %1) to %2.")
            .arg(sourceId)
            .arg(locked ? "LOCK" : "UNLOCK");
    loggable.logMessage(objectName(), QtDebugMsg, msg);


    auto it = startButtons.find(sourceId);
    if (it != startButtons.end())
        it->second->setEnabled(!locked);
}

//---------------------------------------------------------------------------------------
void MainWindow::updatePlaybackState(QMediaPlayer::PlaybackState state)
{
    MediaSource *source = qobject_cast<MediaSource*>(sender());
    if (!source)
        return;

    int sourceId = source->getId();

    QString msg = QString("Update UI elements for source (id %1) due playback state change to %2.")
            .arg(sourceId)
            .arg(mapPlaybackStateToString(state));
    loggable.logMessage(objectName(), QtDebugMsg, msg);

    auto startButtonsIterator = startButtons.find(sourceId);
    if (startButtonsIterator != startButtons.end())
        startButtonsIterator->second->setEnabled(state != QMediaPlayer::PlayingState);

    auto stopButtonsIterator = stopButtons.find(sourceId);
    if (stopButtonsIterator != stopButtons.end())
        stopButtonsIterator->second->setEnabled(state != QMediaPlayer::StoppedState);
}

//---------------------------------------------------------------------------------------
void MainWindow::toggleAudioVolume(bool unmute)
{
    ui->toggleVolumeButton->setIcon(QIcon(unmute ? ":/audio/mute" : ":/audio/unmute"));

    disconnectAudioStream();

    if (!unmute)
        return;

    connectAudioStream();
}

//---------------------------------------------------------------------------------------
void MainWindow::routeAudioStream()
{
    if (ui->toggleVolumeButton->isChecked())
    {
        disconnectAudioStream();
        connectAudioStream();
    }
}

//---------------------------------------------------------------------------------------
void MainWindow::loadActiveConfig()
{
    auto configuration = configManager->getActiveConfiguration();

    if (!configuration)
        return;

    activeConfigId = configuration->id;

    ui->activeConfigurationField->setText(configuration->name);

    loadMosaicWindows();
    loadSources();
}

//---------------------------------------------------------------------------------------
void MainWindow::loadMosaicWindows()
{
    std::vector<MosaicWindowData> windows = configManager->getMosaicWindows(activeConfigId.value());

    for (auto& window : windows)
    {
        MediaViewerWindow *mosaic = new MediaViewerWindow(window.width, window.height, window.name, this);
        mosaicWindows[window.id] = mosaic;

        QAction *act = new QAction(window.name, this);
        mediaWindowsMenu->addAction(act);
        mosaicWindowActions[act] = mosaic;
        connect(act, &QAction::triggered, this, &MainWindow::openMosaicWindow);

        std::unordered_map<int, ServiceData> servicesData = configManager->getServices(window.id);
        for (auto& [id, serviceData] : servicesData)
        {
            auto it = services.find(id);
            if (it != services.end())
            {
                QString msg = QString("Service can be loaded only once. It's already loaded: "
                                      "id %1, sid %2, name \"%3\".")
                        .arg(serviceData.id)
                        .arg(serviceData.sid)
                        .arg(serviceData.name);
                loggable.logMessage(objectName(), QtWarningMsg, msg);
                continue;
            }

            QPoint pos(serviceData.row, serviceData.column);

            auto wgt = mosaic->createTvServiceWidget(serviceData.row, serviceData.column, serviceData.name);

            auto service = std::make_shared<MediaService>(id);
            service->setServiceData(serviceData.sid, serviceData.type, serviceData.name);
            service->setSourceId(serviceData.sourceId);
            service->setWidget(wgt.value_or(nullptr));

            services[id] = std::move(service);
        }
    }

}

//---------------------------------------------------------------------------------------
void MainWindow::loadSources()
{
    std::vector<int> serviceIds;
    serviceIds.reserve(services.size());

    std::transform(services.begin(), services.end(), std::back_inserter(serviceIds), [](auto pair){
        return pair.first;
    });

    auto sourcesData = configManager->getSources(serviceIds);

    sources.reserve(sourcesData.size());
    int row = 0;
    std::map<QString, int> decodedServicesList;
    for (auto& [id, data] : sourcesData)
    {
        auto source = std::make_shared<MediaSource>(data.id, data.name);
        source->setUri(data.uri, data.type);
        source->setAutoRestartConfig(data.autoRestartEnabled, data.autoRestartInterval);

        QLabel *sourceLabel = new QLabel(data.name);
        QPushButton *startButton = new QPushButton(QIcon(":/icons/play"), "");
        QPushButton *stopButton = new QPushButton(QIcon(":/icons/stop"), "");

        startButton->setFixedSize(startButton->minimumSizeHint());
        stopButton->setFixedSize(stopButton->minimumSizeHint());

        stopButton->setEnabled(false);

        startButtons[id] = startButton;
        stopButtons[id] = stopButton;

        sourcesLayout->addWidget(startButton, row, 0);
        sourcesLayout->addWidget(stopButton, row, 1);
        sourcesLayout->addWidget(sourceLabel, row, 2);
        row++;

        connect(startButton, &QPushButton::clicked, source.get(), &MediaSource::start);
        connect(stopButton, &QPushButton::clicked, source.get(), &MediaSource::stop);

        connect(source.get(), &MediaSource::startLockRequired, this, &MainWindow::setStartButtonLocked);
        connect(source.get(), &MediaSource::playbackStateChanged, this, &MainWindow::updatePlaybackState);


        auto servicesInSource = configManager->getServiceIdsBySource(id);
        for (auto serviceId : servicesInSource)
        {
            auto it = services.find(serviceId);
            if (it == services.end())
                continue;

            source->addDecodedService(it->second);

            QString readableServiceName = QString("%1 / %2")
                    .arg(it->second->getName(), source->getName());

            decodedServicesList[readableServiceName] = it->second->getId();
        }

        sources[id] = std::move(source);
    }

    for (auto& [name, id] : decodedServicesList)
    {
        ui->audioStreamsComboBox->addItem(name, id);
    }
}

//---------------------------------------------------------------------------------------
void MainWindow::disconnectAudioStream()
{
    audioOutput->reset();
}

//---------------------------------------------------------------------------------------
void MainWindow::connectAudioStream()
{
    bool ok;

    int serviceId = ui->audioStreamsComboBox->currentData().toInt(&ok);
    if (!ok)
        return;

    auto serviceIterator = services.find(serviceId);
    if (serviceIterator == services.end())
        return;

    int sid = serviceIterator->second->getSid();
    int sourceId = serviceIterator->second->getSourceId();

    auto sourceIterator = sources.find(sourceId);
    if (sourceIterator == sources.end())
        return;

    sourceIterator->second->routeServiceAudio(sid, audioOutput);
}

//---------------------------------------------------------------------------------------
