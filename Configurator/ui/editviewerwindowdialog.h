#ifndef EDITVIEWERWINDOWDIALOG_H
#define EDITVIEWERWINDOWDIALOG_H

#include <QDialog>

#include <QButtonGroup>
#include <QDataWidgetMapper>
#include <QSqlRelationalTableModel>

#include "configstructs.h"
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

    void updateWidgetList();

    void updateSelectedCell();

    void setWidgetToSelectedCell();
    void clearSelectedCell();

signals:
    void modelUpdated();

private:
    void configureMainUi();
    void configureLayoutUi();
    void fillCellTypeList();

    void fillExistingDataFields();
    void createNewRecord();

    void updateServiceData();
    void updateGridLayout();
    void updateServiceWidgetList();

    void updateCellsInDB();
    void processConfigurationChange();
    void removeOutOfRangeCells();
    void insertEmptyCell(int row, int column, int viewer);

    std::optional<CellType> getCellType(int row, int column);
    void setCellType(int row, int column, CellType type);

    void clearCell(int row, int column);
    void clearContentFromCell(int row, int column);
    void clearServicesFromCell(int row, int column);

    void addServiceToCell();

    void fillCell(int row, int column);
    CellType fillCellWithType(int row, int column);
    void fillCellWithServiceData(int row, int column);
    std::optional<QPushButton*> getCellWidget(int row, int column);

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
