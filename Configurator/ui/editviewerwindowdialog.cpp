#include "editviewerwindowdialog.h"
#include "ui_editviewerwindowdialog.h"

#include <QMessageBox>

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelationalDelegate>


//---------------------------------------------------------------------------------------
EditViewerWindowDialog::EditViewerWindowDialog(std::optional<int> id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditViewerWindowDialog),
    viewerId(id)
{
    ui->setupUi(this);
    configureMainUi();

    if (viewerId)
    {
        fillExistingDataFields();
        setWindowTitle(tr("Edit viewer"));
    }
    else
    {
        createNewRecord();
        setWindowTitle(tr("Add viewer"));
    }

    configureLayoutUi();
    updateServiceData();

    connect(ui->applyButton, &QPushButton::clicked, this, &EditViewerWindowDialog::checkAndSubmitMainData);

}

//---------------------------------------------------------------------------------------
EditViewerWindowDialog::~EditViewerWindowDialog()
{
    delete ui;
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::checkAndSubmitMainData()
{
    if (ui->nameLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("You must enter viewer name!"), QMessageBox::Close);
        return;
    }
    else if (ui->configComboBox->currentIndex() == -1)
    {
        QMessageBox::warning(this, tr("Warning"), tr("You must select configuration!"), QMessageBox::Close);
        return;
    }

    if (!viewerMapper->submit())
    {
        QString msg = QString("FAILED submitting of viewer data: %1").arg(viewerModel->lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }


    if (!viewerId)
    {
        QModelIndex idx = viewerModel->index(viewerMapper->currentIndex(), viewerModel->fieldIndex("id"));
        viewerId = viewerModel->data(idx).toInt();
    }

    updateServiceData();
    emit modelUpdated();
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::updateWidgetList()
{
    ui->widgetList->clear();

    CellType cellType = static_cast<CellType>(ui->widgetTypeComboBox->currentData().toInt());

    switch (cellType)
    {
    case CellType::TvWidget:
    case CellType::RadioWidgets:
        updateServiceWidgetList();
        break;
    default:
        break;
    }

    if (cellType == CellType::RadioWidgets)
        ui->setButton->setText(tr("Add"));
    else
        ui->setButton->setText(tr("Set"));
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::updateSelectedCell()
{
    QGridLayout *grid = static_cast<QGridLayout*>(ui->layoutGroupBox->layout());
    if (selectedCell)
    {
        QLayoutItem *item = grid->itemAtPosition(selectedCell->y(), selectedCell->x());
        if (item)
        {
            QPushButton *btn = qobject_cast<QPushButton*>(item->widget());
            if (btn)
                btn->setStyleSheet(baseCellStyle);
        }
    }

    selectedCell.reset();
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton)
        return;

    for (int row = 0; row < grid->rowCount(); ++row)
    {
        for (int col = 0; col < grid->columnCount(); ++col)
        {
            if (clickedButton == grid->itemAtPosition(row, col)->widget())
            {
                selectedCell = QPoint(col, row);
                clickedButton->setStyleSheet(baseCellStyle + selectedCellStyle);
                break;
            }
        }
    }
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::setWidgetToSelectedCell()
{
    if (!selectedCell || !viewerId)
        return;

    int row = selectedCell->y();
    int column = selectedCell->x();
    std::optional<CellType> optPrevType = getCellType(row, column);

    CellType cellType = static_cast<CellType>(ui->widgetTypeComboBox->currentData().toInt());

    if (optPrevType)
    {
        if (optPrevType.value() != cellType)
        {
            clearContentFromCell(row, column);
            setCellType(row, column, cellType);
        }
        else if (cellType != CellType::RadioWidgets)
        {
            clearContentFromCell(row, column);
        }
    }
    else
    {
        setCellType(row, column, cellType);
    }

    switch (cellType)
    {
    case CellType::TvWidget:
    case CellType::RadioWidgets:
        addServiceToCell();
        break;
    default:
        break;
    }

    fillCell(row, column);
    updateWidgetList();
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::clearSelectedCell()
{
    if (!selectedCell || !viewerId)
        return;

    int row = selectedCell->y();
    int column = selectedCell->x();

    clearCell(row, column);
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::configureMainUi()
{
    viewerModel = new QSqlRelationalTableModel(this);
    viewerModel->setTable("mosaic_viewer");

    int nameFieldNo = viewerModel->fieldIndex("name");
    int configFieldNo = viewerModel->fieldIndex("configuration");
    int widthFieldNo = viewerModel->fieldIndex("width");
    int heightFieldNo = viewerModel->fieldIndex("height");
    int monitorFieldNo = viewerModel->fieldIndex("monitor");

    viewerModel->setRelation(configFieldNo, QSqlRelation("configuration", "id", "name, mosaic_viewer.configuration"));

    QSqlTableModel *configModel = viewerModel->relationModel(configFieldNo);
    configModel->setSort(configModel->fieldIndex("name"), Qt::AscendingOrder);
    ui->configComboBox->setModel(configModel);
    ui->configComboBox->setModelColumn(configModel->fieldIndex("name"));
    configModel->select();

    viewerMapper = new QDataWidgetMapper(this);
    viewerMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    viewerMapper->setModel(viewerModel);
    viewerMapper->setItemDelegate(new QSqlRelationalDelegate(this));
    viewerMapper->addMapping(ui->nameLineEdit, nameFieldNo);
    viewerMapper->addMapping(ui->configComboBox, configFieldNo);
    viewerMapper->addMapping(ui->widthSpinBox, widthFieldNo);
    viewerMapper->addMapping(ui->heightSpinBox, heightFieldNo);
    viewerMapper->addMapping(ui->monitorSpinBox, monitorFieldNo);

    viewerModel->select();
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::configureLayoutUi()
{
    QGridLayout *grid = new QGridLayout();
    grid->setHorizontalSpacing(0);
    grid->setVerticalSpacing(0);

    ui->layoutGroupBox->setLayout(grid);

    baseCellStyle = "background-color: #000; color: #FFF; font-weight: bold; border: 1px solid #777;";
    selectedCellStyle = "border: 1px solid #FF0;";

    connect(ui->widgetTypeComboBox, &QComboBox::currentIndexChanged, this, &EditViewerWindowDialog::updateWidgetList);
    fillCellTypeList();

    connect(ui->setButton, &QPushButton::clicked, this, &EditViewerWindowDialog::setWidgetToSelectedCell);
    connect(ui->clearButton, &QPushButton::clicked, this, &EditViewerWindowDialog::clearSelectedCell);
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::fillCellTypeList()
{
    QSqlQuery q;
    bool ok;

    ok = q.prepare("SELECT id, name FROM cell_type WHERE id <> :empty ORDER BY id");

    if (!ok)
    {
        QString msg = QString("FAILED SQL query preparing for cell type list retrieving: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    q.bindValue(":empty", static_cast<int>(CellType::Empty));

    if (!q.exec())
    {
        QString msg = QString("FAILED SQL query executing for cell type list retrieving: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    while (q.next())
    {
        QString name = q.value("name").toString();
        int id = q.value("id").toInt();
        ui->widgetTypeComboBox->addItem(name, id);
    }
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::fillExistingDataFields()
{
    int idFieldNo = viewerModel->fieldIndex("id");
    for(int row = 0; row < viewerModel->rowCount(); ++row)
    {
        QSqlRecord record = viewerModel->record(row);
        if(record.value(idFieldNo).toInt() == viewerId.value())
        {
            viewerMapper->setCurrentIndex(row);
            break;
        }
    }
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::createNewRecord()
{
    viewerModel->insertRow(0);
    viewerMapper->setCurrentIndex(0);

    ui->configComboBox->setCurrentIndex(-1);
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::updateServiceData()
{
    updateCellsInDB();
    updateGridLayout();
    updateWidgetList();
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::updateGridLayout()
{
    qDeleteAll(ui->layoutGroupBox->findChildren<QPushButton*>());

    int rowCount = ui->heightSpinBox->value();
    int columnCount = ui->widthSpinBox->value();

    QGridLayout *grid = static_cast<QGridLayout*>(ui->layoutGroupBox->layout());

    static QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    for (int row = 0; row < rowCount; ++row)
    {
        for (int col = 0; col < columnCount; ++col)
        {
            QPushButton *cell = new QPushButton();
            cell->setSizePolicy(policy);
            cell->setStyleSheet(baseCellStyle);
            grid->addWidget(cell, row, col);
            fillCell(row, col);
            connect(cell, &QPushButton::clicked, this, &EditViewerWindowDialog::updateSelectedCell);
        }
    }
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::updateServiceWidgetList()
{
    QSqlQuery q;
    bool ok;

    ok = q.prepare("SELECT serv.id AS service_id, serv.name AS service_name, src.name AS source_name "
                   "FROM service AS serv "
                   "INNER JOIN source AS src ON src.id = serv.source "
                   "WHERE src.configuration = :configId AND serv.type = :srvType "
                   "AND service_id NOT IN (SELECT smv.service FROM service_mosaic_viewer AS smv) "
                   "ORDER BY serv.name");

    if (!ok)
    {
        QString msg = QString("FAILED SQL query preparing for service list retrieving: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    QModelIndex idx = viewerModel->index(viewerMapper->currentIndex(), viewerModel->fieldIndex("configuration"));
    int configId = viewerModel->data(idx).toInt();

    q.bindValue(":configId", configId);
    q.bindValue(":srvType", ui->widgetTypeComboBox->currentData().toInt());

    if (!q.exec())
    {
        QString msg = QString("FAILED SQL query executing for service list retrieving: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    while (q.next())
    {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(q.value("service_name").toString());
        item->setToolTip(QString("%1 / %2")
                         .arg(q.value("service_name").toString(),
                              q.value("source_name").toString()));
        item->setData(Qt::UserRole, q.value("service_id").toInt());
        ui->widgetList->addItem(item);
    }
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::updateCellsInDB()
{
    processConfigurationChange();
    removeOutOfRangeCells();
}

//---------------------------------------------------------------------------------------
/// NOTE: this method will be removed
void EditViewerWindowDialog::processConfigurationChange()
{
    if (!viewerId)
        return;

    QSqlQuery q;
    bool ok;
    QString msg;

    // find coordinates of cells containing services from another configuration
    ok = q.prepare("SELECT mc.cell_row, mc.cell_column "
                   "FROM mosaic_cell AS mc "
                   "INNER JOIN service_mosaic_viewer AS smv ON smv.cell = mc.id "
                   "WHERE smv.service IN "
                   "(SELECT serv.id "
                   "FROM service AS serv "
                   "INNER JOIN source AS src ON serv.source = src.id "
                   "WHERE src.configuration <> :configId) "
                   "AND mc.viewer = :viewer");

    if (!ok)
    {
        msg = QString("FAILED SQL query preparing for finding coordinates of cells containing services from another configuration: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return ;
    }

    QModelIndex idx = viewerModel->index(viewerMapper->currentIndex(), viewerModel->fieldIndex("configuration"));
    int configId = viewerModel->data(idx).toInt();

    q.bindValue(":viewer", viewerId.value());
    q.bindValue(":configId", configId);

    if (!q.exec())
    {
        msg = QString("FAILED SQL query executing for find coordinates of cells containing services from another configuration: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    while (q.next())
    {
        int row = q.value("cell_row").toInt();
        int column = q.value("cell_column").toInt();
        clearCell(row, column);
    }
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::removeOutOfRangeCells()
{
    if (!viewerId)
        return;

    QSqlQuery q;
    bool ok;
    QString msg;

    ok = q.prepare("DELETE FROM mosaic_cell "
                   "WHERE viewer = :viewer "
                   "AND (cell_row >= :height OR cell_column >= :width)");

    if (!ok)
    {
        msg = QString("FAILED SQL query preparing for deleting cells on viewer size change: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return ;
    }

    q.bindValue(":width", ui->widthSpinBox->value());
    q.bindValue(":height", ui->heightSpinBox->value());
    q.bindValue(":viewer", viewerId.value());

    if (!q.exec())
    {
        msg = QString("FAILED SQL query executing for deleting cells on viewer size change: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::insertEmptyCell(int row, int column, int viewer)
{
    QSqlQuery q;
    bool ok;
    QString msg;

    ok = q.prepare("INSERT INTO mosaic_cell "
                   "(viewer, cell_row, cell_column, cell_type) "
                   "VALUES "
                   "(:viewer, :row, :column, 0)");

    if (!ok)
    {
        msg = QString("FAILED SQL query preparing for empty cell inserting: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return ;
    }

    q.bindValue(":viewer", viewer);
    q.bindValue(":row", row);
    q.bindValue(":column", column);

    if (!q.exec())
    {
        msg = QString("FAILED SQL query executing for empty cell inserting: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

}

//---------------------------------------------------------------------------------------
std::optional<CellType> EditViewerWindowDialog::getCellType(int row, int column)
{
    if (!viewerId)
        return std::nullopt;

    QSqlQuery q;
    bool ok;
    QString msg;

    ok = q.prepare("SELECT cell_type FROM mosaic_cell "
                   "WHERE viewer = :viewer AND cell_row = :row AND cell_column = :column");

    if (!ok)
    {
        msg = QString("FAILED SQL query preparing for type to cell setup: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return std::nullopt;
    }

    q.bindValue(":viewer", viewerId.value());
    q.bindValue(":row", row);
    q.bindValue(":column", column);

    if (!q.exec())
    {
        msg = QString("FAILED SQL query executing for type to cell setup: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return std::nullopt;
    }

    if(!q.first())
    {
        insertEmptyCell(row, column, viewerId.value());
        return CellType::Empty;
    }

    return static_cast<CellType>(q.value("cell_type").toInt());
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::setCellType(int row, int column, CellType type)
{
    if (!viewerId)
        return;

    QSqlQuery q;
    bool ok;
    QString msg;

    ok = q.prepare("UPDATE mosaic_cell SET cell_type = :type "
                   "WHERE viewer = :viewer AND cell_row = :row AND cell_column = :column");

    if (!ok)
    {
        msg = QString("FAILED SQL query preparing for cell type changing: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return ;
    }

    q.bindValue(":type", static_cast<int>(type));
    q.bindValue(":viewer", viewerId.value());
    q.bindValue(":row", row);
    q.bindValue(":column", column);

    if (!q.exec())
    {
        msg = QString("FAILED SQL query executing for cell type changing: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::clearCell(int row, int column)
{
    clearContentFromCell(row, column);
    setCellType(row, column, CellType::Empty);
    fillCell(row, column);
    updateWidgetList();
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::clearContentFromCell(int row, int column)
{
    std::optional<CellType> prevType = getCellType(row, column);

    if (!prevType)
        return;

    switch (prevType.value())
    {
    case CellType::TvWidget:
    case CellType::RadioWidgets:
        clearServicesFromCell(row, column);
        break;
    default:
        break;
    }
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::clearServicesFromCell(int row, int column)
{
    if (!viewerId)
        return;

    QSqlQuery q;
    bool ok;
    QString msg;

    ok = q.prepare("DELETE FROM service_mosaic_viewer "
                   "WHERE cell IN "
                   "(SELECT id FROM mosaic_cell "
                   "WHERE viewer = :viewer AND cell_row = :row AND cell_column = :column)");

    if (!ok)
    {
        msg = QString("FAILED SQL query preparing for cell clearing: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return ;
    }

    q.bindValue(":viewer", viewerId.value());
    q.bindValue(":row", row);
    q.bindValue(":column", column);

    if (!q.exec())
    {
        msg = QString("FAILED SQL query executing for cell clearing: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::addServiceToCell()
{
    if (!selectedCell || !viewerId)
        return;

    auto selectedServices = ui->widgetList->selectedItems();
    if (selectedServices.isEmpty())
        return;

    int serviceId = selectedServices.first()->data(Qt::UserRole).toInt();

    QSqlQuery q;
    bool ok;
    QString msg;

    ok = q.prepare("INSERT OR REPLACE INTO service_mosaic_viewer "
                   "(service, cell) "
                   "VALUES "
                   "(:service, "
                   "(SELECT id FROM mosaic_cell "
                   "WHERE viewer = :viewer AND cell_row = :row AND cell_column = :column))");

    if (!ok)
    {
        msg = QString("FAILED SQL query preparing for service to cell setup: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return ;
    }

    q.bindValue(":service", serviceId);
    q.bindValue(":viewer", viewerId.value());
    q.bindValue(":row", selectedCell->y());
    q.bindValue(":column", selectedCell->x());

    if (!q.exec())
    {
        msg = QString("FAILED SQL query executing for service to cell setup: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::fillCell(int row, int column)
{
    CellType cellType = fillCellWithType(row, column);

    switch (cellType)
    {
    case CellType::TvWidget:
    case CellType::RadioWidgets:
        fillCellWithServiceData(row, column);
        break;
    default:
        break;
    }
}

//---------------------------------------------------------------------------------------
CellType EditViewerWindowDialog::fillCellWithType(int row, int column)
{
    auto optCell = getCellWidget(row, column);
    if (!optCell)
        return CellType::Empty;
    QPushButton *cell = optCell.value();

    QSqlQuery q;
    bool ok;

    std::optional<CellType> type = getCellType(row, column);

    if (!type)
        return CellType::Empty;

    ok = q.prepare("SELECT name FROM cell_type WHERE id = :type LIMIT 1");

    if (!ok)
    {
        QString msg = QString("FAILED SQL query preparing for cell type retrieving: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return CellType::Empty;
    }

    q.bindValue(":type", static_cast<int>(type.value()));

    if (!q.exec())
    {
        QString msg = QString("FAILED SQL query executing for cell type retrieving: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return CellType::Empty;
    }

    if(!q.first())
        return CellType::Empty;

    QString typeName = q.value("name").toString();
    cell->setText(typeName);

    return type.value();
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::fillCellWithServiceData(int row, int column)
{
    auto optCell = getCellWidget(row, column);
    if (!optCell)
        return;
    QPushButton *cell = optCell.value();

    QSqlQuery q;
    bool ok;

    ok = q.prepare("SELECT serv.name AS service_name, serv.type AS service_type, "
                   "src.name AS source_name "
                   "FROM service AS serv "
                   "INNER JOIN service_mosaic_viewer AS smv ON serv.id = smv.service "
                   "INNER JOIN mosaic_cell AS mc ON smv.cell = mc.id "
                   "INNER JOIN source AS src ON src.id = serv.source "
                   "WHERE mc.viewer = :viewer AND mc.cell_row = :row AND mc.cell_column = :column "
                   "ORDER BY serv.name");

    if (!ok)
    {
        QString msg = QString("FAILED SQL query preparing for service retrieving: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    q.bindValue(":viewer", viewerId.value());
    q.bindValue(":row", row);
    q.bindValue(":column", column);

    if (!q.exec())
    {
        QString msg = QString("FAILED SQL query executing for service retrieving: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    int serviceCount = 0;
    while(q.next())
    {
        int serviceType = q.value("service_type").toInt();
        QString serviceName = q.value("service_name").toString();
        QString sourceName = q.value("source_name").toString();

        // the cell already contains radio services
        if (serviceType == ServiceType::TV && serviceCount > 0)
            continue;

        cell->setText(cell->text() + '\n' + serviceName);
        cell->setToolTip(cell->toolTip() + '\n' + QString("%1 / %2").arg(serviceName, sourceName));

        // only one TV service in the cell
        if (serviceType == ServiceType::TV)
            break;

        ++serviceCount;
    }

    return;
}

//---------------------------------------------------------------------------------------
std::optional<QPushButton*> EditViewerWindowDialog::getCellWidget(int row, int column)
{
    QGridLayout *grid = static_cast<QGridLayout*>(ui->layoutGroupBox->layout());
    QLayoutItem *item = grid->itemAtPosition(row, column);
    if (!item)
        return std::nullopt;

    QPushButton *cell = qobject_cast<QPushButton*>(item->widget());
    if (!cell)
        return std::nullopt;

    return cell;
}

//---------------------------------------------------------------------------------------
