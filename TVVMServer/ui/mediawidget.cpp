#include "mediawidget.h"

#include <QLayout>

//---------------------------------------------------------------------------------------
MediaWidget::MediaWidget(const QString &serviceName, QWidget *parent)
    : QWidget{parent}
{
    serviceNameLabel = new QLabel(serviceName);
}

//---------------------------------------------------------------------------------------
void MediaWidget::updateServiceName(const QString &name)
{
    serviceNameLabel->setText(name);
}

//---------------------------------------------------------------------------------------
void MediaWidget::updateAudioIndicatorsCount(int audioChannelsCount)
{
    while (audioIndicators.size() > audioChannelsCount)
    {
        AudioLevelWidget *indicator = audioIndicators.back();
        audioIndicators.pop_back();
        delete indicator;
    }

    while (audioIndicators.size() < audioChannelsCount)
    {
        AudioLevelWidget *indicator = new AudioLevelWidget();
        audioIndicators.push_back(indicator);

        if (audioIndicatorsLayout)
            audioIndicatorsLayout->addWidget(indicator);
    }
}

//---------------------------------------------------------------------------------------
void MediaWidget::updateAudioIndicatorLevels(const std::vector<double> &levels)
{
    for (int i = 0; i < levels.size() && i < audioIndicators.size(); ++i)
    {
        audioIndicators[i]->setLevel(levels[i]);
    }
}

//---------------------------------------------------------------------------------------
