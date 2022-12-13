#ifndef MEDIAWIDGET_H
#define MEDIAWIDGET_H

#include <QWidget>

#include <QLabel>

#include "audiolevelwidget.h"

class MediaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MediaWidget(const QString &serviceName, QWidget *parent = nullptr);

public slots:
    void updateServiceName(const QString &name);

    void updateAudioIndicatorsCount(int audioChannelsCount);
    void updateAudioIndicatorLevels(const std::vector<double> &levels);

protected:
    QLabel *serviceNameLabel{nullptr};
    QLayout *audioIndicatorsLayout{nullptr};
    std::vector<AudioLevelWidget*> audioIndicators;

};

#endif // MEDIAWIDGET_H
