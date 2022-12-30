#include "mediaviewerwindow.h"

#include "tvservicewidget.h"

//---------------------------------------------------------------------------------------
MediaViewerWindow::MediaViewerWindow(uint8_t w, uint8_t h, const QString &name, QWidget *parent)
    : QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint)
    , numberOfColumns(w)
    , numberOfRows(h)

{

    setObjectName("MediaViewerWindow " + name);
    setWindowTitle(name);
    gridLayout = new QGridLayout();
//    gridLayout->setSpacing(5);
    gridLayout->setHorizontalSpacing(1);
    gridLayout->setVerticalSpacing(1);

    for (size_t row = 0; row < numberOfRows; ++row)
        gridLayout->setRowStretch(row, 1);
    for (size_t col = 0; col < numberOfColumns; ++col)
        gridLayout->setColumnStretch(col, 1);

    setLayout(gridLayout);
    setStyleSheet("MediaViewerWindow{background-color: black;}");
    setMinimumSize(640, 480);
}

//---------------------------------------------------------------------------------------
void MediaViewerWindow::setMonitorNumber(int num)
{
    monitorNumber = num;
}

//---------------------------------------------------------------------------------------
int MediaViewerWindow::getMonitorNumber() const
{
    return monitorNumber;
}

//---------------------------------------------------------------------------------------
std::optional<TvServiceWidget*> MediaViewerWindow::createTvServiceWidget(uint8_t row, uint8_t column,
                                                                          const QString &serviceName)
{
    if (column >= numberOfColumns || row >= numberOfRows)
    {
        loggable.logMessage(objectName(), QtWarningMsg,
                            "FAILED adding TV service widget: coordinates out of range!");
        return std::nullopt;
    }

    TvServiceWidget *wgt = new TvServiceWidget(serviceName);
    gridLayout->addWidget(wgt, row, column);

    clearCell(row ,column);
    widgets[QPoint(row, column)] = wgt;

    return wgt;
}

//---------------------------------------------------------------------------------------
std::optional<RadioServiceWidget*> MediaViewerWindow::createRadioServiceWidget(uint8_t row,
                                                                               uint8_t column,
                                                                               const QString &serviceName)
{
    if (column >= numberOfColumns || row >= numberOfRows)
    {
        loggable.logMessage(objectName(), QtWarningMsg,
                            "FAILED adding radio service widget: coordinates out of range!");
        return std::nullopt;
    }


    QVBoxLayout *cellLayout{nullptr};
    QLayoutItem *item = gridLayout->itemAtPosition(row, column);
    if (!item)
    {
        QWidget *cellWidget = new QWidget();
        cellLayout = new QVBoxLayout();
        cellLayout->addStretch(1);
        cellWidget->setLayout(cellLayout);
        gridLayout->addWidget(cellWidget, row, column);
    }
    else
    {
        QWidget *cellWidget = qobject_cast<QWidget*>(item->widget());
        if (!cellWidget)
        {
            loggable.logMessage(objectName(), QtWarningMsg,
                                "ERROR! Cell for radio widgets contains the widget that not QWidget.");
            return std::nullopt;
        }
        cellLayout = static_cast<QVBoxLayout*>(cellWidget->layout());
    }

    RadioServiceWidget *wgt = new RadioServiceWidget(serviceName);
    cellLayout->addWidget(wgt);

    return wgt;
}

//---------------------------------------------------------------------------------------
void MediaViewerWindow::clearCell(uint8_t row, uint8_t column)
{
    QPoint p{row, column};
    auto it = widgets.find(p);
    if (it != widgets.end())
    {
        delete it.value();
        widgets.remove(p);
    }
}

//---------------------------------------------------------------------------------------
void MediaViewerWindow::mouseDoubleClickEvent([[maybe_unused]] QMouseEvent *ev)
{
    if(windowState() & Qt::WindowFullScreen)
        showNormal();
    else
        showFullScreen();
}

//---------------------------------------------------------------------------------------
