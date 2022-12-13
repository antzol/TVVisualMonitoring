#ifndef EDITVIEWERWINDOWDIALOG_H
#define EDITVIEWERWINDOWDIALOG_H

#include <QDialog>

#include <QButtonGroup>
#include <QDataWidgetMapper>
#include <QSqlRelationalTableModel>

#include "loggable.h"

namespace Ui {
class EditViewerWindowDialog;
}

class EditViewerWindowDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditViewerWindowDialog(std::optional<int> id, QWidget *parent = nullptr);
    ~EditViewerWindowDialog();

public slots:
    void checkAndSubmitMainData();

    void updateSelectedCell();

    void setServiceToCell();
    void clearCell();

signals:
    void modelUpdated();

private:
    void configureMainUi();
    void configureLayoutUi();

    void fillExistingDataFields();
    void createNewRecord();

    void updateServiceData();
    void updateGridLayout();
    void updateServiceList();
    void removeInvalidCellsFromDB();

    std::tuple<QString, QString> getServiceByCoordinate(int row, int column);

    Ui::EditViewerWindowDialog *ui;
    Loggable loggable;
    QSqlRelationalTableModel *viewerModel;
    QDataWidgetMapper *viewerMapper;

    std::optional<int> viewerId;

    std::optional<QPoint> selectedCell;

    QString baseCellStyle;
    QString selectedCellStyle;
};

#endif // EDITVIEWERWINDOWDIALOG_H
