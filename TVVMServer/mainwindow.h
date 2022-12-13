#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QHBoxLayout>

#include "configmanager.h"
#include "configstructs.h"

#include "loggable.h"
#include "mediaviewerwindow.h"
#include "mediaservice.h"
#include "mediasource.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void openMosaicWindow();

    void setStartButtonLocked(bool locked);
    void updatePlaybackState(QMediaPlayer::PlaybackState state);

signals:


private:
    void loadActiveConfig();
    void loadMosaicWindows();
    void loadSources();


    Ui::MainWindow *ui;
    QGridLayout *sourcesLayout;

    QMenu *mediaWindowsMenu;

    ConfigManager *configManager{nullptr};

    std::optional<int> activeConfigId;

    std::unordered_map<int, MediaViewerWindow*> mosaicWindows;
    std::unordered_map<QAction*, MediaViewerWindow*> mosaicWindowActions;

    // key - service id in the DB
    std::unordered_map<int, std::shared_ptr<MediaService>> services;

    // key - source id in the DB
    std::unordered_map<int, std::shared_ptr<MediaSource>> sources;
    std::unordered_map<int, QPushButton*> startButtons;
    std::unordered_map<int, QPushButton*> stopButtons;


    Loggable loggable;

};
#endif // MAINWINDOW_H
