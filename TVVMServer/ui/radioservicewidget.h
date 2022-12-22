#ifndef RADIOSERVICEWIDGET_H
#define RADIOSERVICEWIDGET_H

#include "mediawidget.h"

#include <QVBoxLayout>

class RadioServiceWidget : public MediaWidget
{
    Q_OBJECT
public:
    explicit RadioServiceWidget(const QString &serviceName, QWidget *parent = nullptr);

private:
    QVBoxLayout *mediaLayout;
};

#endif // RADIOSERVICEWIDGET_H
