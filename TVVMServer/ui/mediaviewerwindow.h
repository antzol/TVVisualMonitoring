#ifndef MEDIAVIEWERWINDOW_H
#define MEDIAVIEWERWINDOW_H

#include <QDialog>

#include <QGridLayout>

#include "loggable.h"
#include "tvservicewidget.h"

class MediaViewerWindow : public QDialog
{
    Q_OBJECT
public:
    MediaViewerWindow(uint8_t w, uint8_t h, const QString &name, QWidget *parent = nullptr);

    std::optional<TvServiceWidget*> createTvServiceWidget(uint8_t row, uint8_t column, const QString &serviceName);


    void clearCell(uint8_t row, uint8_t column);

protected:
    void mouseDoubleClickEvent(QMouseEvent *ev);

private:
    int numberOfColumns;
    int numberOfRows;

    QGridLayout *gridLayout;

    QHash<QPoint, QWidget*> widgets;

    Loggable loggable;
};

#endif // MEDIAVIEWERWINDOW_H
