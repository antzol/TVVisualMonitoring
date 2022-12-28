#include "radioservicewidget.h"

//---------------------------------------------------------------------------------------
RadioServiceWidget::RadioServiceWidget(const QString &serviceName, QWidget *parent)
    : MediaWidget(serviceName, parent)
{
    serviceType = ServiceType::Radio;
    mediaLayout = new QVBoxLayout();
    mediaLayout->setAlignment(Qt::AlignBottom);

    serviceNameLabel->setStyleSheet("QLabel{color: white; font: bold 14px arial,sans-serif;}");

    QVBoxLayout *wgtLayout = new QVBoxLayout();
    wgtLayout->setSpacing(1);
    wgtLayout->addLayout(mediaLayout);
    wgtLayout->addWidget(serviceNameLabel, 0, Qt::AlignHCenter);

    audioIndicatorsLayout = new QVBoxLayout();
    audioIndicatorsLayout->setSpacing(1);
    mediaLayout->addLayout(audioIndicatorsLayout);

    setLayout(wgtLayout);
}

//---------------------------------------------------------------------------------------
