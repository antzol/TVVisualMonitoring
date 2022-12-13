#include "editviewerwindowdialog.h"
#include "ui_editviewerwindowdialog.h"

#include <QMessageBox>

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelationalDelegate>

#include "configstructs.h"

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
void EditViewerWindowDialog::setServiceToCell()
{
    if (!selectedCell || !viewerId)
        return;

    auto selectedServices = ui->serviceListWidget->selectedItems();
    if (selectedServices.isEmpty())
        return;

    int serviceId = selectedServices.first()->data(Qt::UserRole).toInt();

    QSqlQuery q;
    bool ok;
    QString msg;

    ok = q.prepare("INSERT OR REPLACE INTO service_mosaic_viewer "
                   "(service, viewer, column, row) "
                   "VALUES "
                   "(:service, :viewer, :column, :row)");

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

    QGridLayout *grid = static_cast<QGridLayout*>(ui->layoutGroupBox->layout());
    QLayoutItem *item = grid->itemAtPosition(selectedCell->y(), selectedCell->x());
    if (item)
    {
        QPushButton *btn = qobject_cast<QPushButton*>(item->widget());
        if (btn)
        {
            auto serviceData = getServiceByCoordinate(selectedCell->y(), selectedCell->x());
            btn->setText(std::get<0>(serviceData));
            if (!std::get<0>(serviceData).isEmpty())
                btn->setToolTip(QString("%1 / %2").arg(std::get<0>(serviceData), std::get<1>(serviceData)));
        }
    }
}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::clearCell()
{
    if (!selectedCell || !viewerId)
        return;

    QSqlQuery q;
    bool ok;
    QString msg;

    ok = q.prepare("DELETE FROM service_mosaic_viewer "
                   "WHERE viewer = :viewer AND row = :row AND column = :column");

    if (!ok)
    {
        msg = QString("FAILED SQL query preparing for cell clearing: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return ;
    }

    q.bindValue(":viewer", viewerId.value());
    q.bindValue(":row", selectedCell->y());
    q.bindValue(":column", selectedCell->x());

    if (!q.exec())
    {
        msg = QString("FAILED SQL query executing for cell clearing: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    QGridLayout *grid = static_cast<QGridLayout*>(ui->layoutGroupBox->layout());
    QLayoutItem *item = grid->itemAtPosition(selectedCell->y(), selectedCell->x());
    if (item)
    {
        QPushButton *btn = qobject_cast<QPushButton*>(item->widget());
        if (btn)
        {
            btn->setText("");
            btn->setToolTip("");
        }
    }
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

    connect(ui->setButton, &QPushButton::clicked, this, &EditViewerWindowDialog::setServiceToCell);
    connect(ui->clearButton, &QPushButton::clicked, this, &EditViewerWindowDialog::clearCell);
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
    removeInvalidCellsFromDB();
    updateGridLayout();
    updateServiceList();
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
            auto serviceData = getServiceByCoordinate(row, col);
            cell->setText(std::get<0>(serviceData));
            if (!std::get<0>(serviceData).isEmpty())
                cell->setToolTip(QString("%1 / %2").arg(std::get<0>(serviceData), std::get<1>(serviceData)));
            grid->addWidget(cell, row, col);
            connect(cell, &QPushButton::clicked, this, &EditViewerWindowDialog::updateSelectedCell);
        }
    }

}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::updateServiceList()
{
    ui->serviceListWidget->clear();

    QSqlQuery q;
    bool ok;

    ok = q.prepare("SELECT serv.id AS service_id, serv.name AS service_name, src.name AS source_name "
                   "FROM service AS serv "
                   "INNER JOIN source AS src ON src.id = serv.source "
                   "WHERE src.configuration = :configId AND serv.type = :srvType "
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
    q.bindValue(":srvType", ServiceType::TV);

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
        ui->serviceListWidget->addItem(item);
    }

}

//---------------------------------------------------------------------------------------
void EditViewerWindowDialog::removeInvalidCellsFromDB()
{
    if (!viewerId)
        return;

    QSqlQuery q;
    bool ok;
    QString msg;

    // for configuration change
    ok = q.prepare("DELETE FROM service_mosaic_viewer "
                   "WHERE service IN "
                   "(SELECT serv.id "
                   "FROM service AS serv "
                   "INNER JOIN source AS src ON serv.source = src.id "
                   "WHERE src.configuration <> :configId) "
                   "AND viewer = :viewer");

    if (!ok)
    {
        msg = QString("FAILED SQL query preparing for deleting cells on configuration change: %1")
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
        msg = QString("FAILED SQL query executing for deleting cells on configuration change: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    // for viewer size reducing
    ok = q.prepare("DELETE FROM service_mosaic_viewer "
                   "WHERE viewer = :viewer "
                   "AND (row >= :height OR column >= :width)");

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
std::tuple<QString, QString> EditViewerWindowDialog::getServiceByCoordinate(int row, int column)
{
    QSqlQuery q;
    bool ok;

    ok = q.prepare("SELECT serv.name AS service_name, src.name AS source_name "
                   "FROM service AS serv "
                   "INNER JOIN service_mosaic_viewer AS smv ON serv.id = smv.service "
                   "INNER JOIN source AS src ON src.id = serv.source "
                   "WHERE smv.viewer = :viewer AND smv.row = :row AND smv.column = :column "
                   "LIMIT 1");

    if (!ok)
    {
        QString msg = QString("FAILED SQL query preparing for service retrieving: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return std::make_tuple("", "");
    }

    q.bindValue(":viewer", viewerId.value());
    q.bindValue(":row", row);
    q.bindValue(":column", column);

    if (!q.exec())
    {
        QString msg = QString("FAILED SQL query executing for service retrieving: %1")
                .arg(q.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return std::make_tuple("", "");
    }

    if (!q.first())
        return std::make_tuple("", "");

    QString serviceName = q.value("service_name").toString();
    QString sourceName = q.value("source_name").toString();

    return std::make_tuple(serviceName, sourceName);
}

//---------------------------------------------------------------------------------------
