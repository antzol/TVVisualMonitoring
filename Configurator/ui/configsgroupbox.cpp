#include "configsgroupbox.h"
#include "ui_configsgroupbox.h"

#include <QMessageBox>

#include <QSqlError>
#include <QSqlQuery>

//---------------------------------------------------------------------------------------
ConfigsGroupBox::ConfigsGroupBox(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ConfigsGroupBox)
{
    ui->setupUi(this);
    init();
}

//---------------------------------------------------------------------------------------
ConfigsGroupBox::~ConfigsGroupBox()
{
    delete ui;
}

//---------------------------------------------------------------------------------------
void ConfigsGroupBox::addConfig()
{
    int row = model->rowCount();
    model->insertRow(row);
    QModelIndex idx = model->index(row, model->fieldIndex("name"));
    ui->tableView->setCurrentIndex(idx);
    ui->tableView->edit(idx);
    model->submit();
}

//---------------------------------------------------------------------------------------
void ConfigsGroupBox::deleteConfig()
{
    loggable.logMessage(objectName(), QtDebugMsg, "Delete configuration...");

    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();

    int size = indexes.size();

    if(size)
    {
        int n = QMessageBox::warning(this, tr("Attention"),
                                     tr("Do you want delete %1 configuration(s) from the database?\n"
                                        "All data associated with them will also be deleted!")
                                     .arg(size),
                                     QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if(n == QMessageBox::No)
            return;
    }

    for (auto& idx : indexes)
        model->removeRows(idx.row(), 1);

    updateData();
}

//---------------------------------------------------------------------------------------
void ConfigsGroupBox::activateConfig()
{
    loggable.logMessage(objectName(), QtDebugMsg, "Change active configuration...");

    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedIndexes();

    int size = indexes.size();

    if(!size)
    {
        QMessageBox::warning(this, tr("Warning"), tr("You need select one configuration for activating!"));
        return;
    }

    int idFieldNo = model->fieldIndex("id");
    int row = indexes.first().row();
    int id = model->data(model->index(row, idFieldNo)).toInt();

    QSqlQuery query;

    if (!query.exec("UPDATE configuration SET is_active = 0 WHERE is_active = 1"))
    {
        QString msg = QString("FAILED resetting of previous active configuration: %1")
                .arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    bool ok = query.prepare("UPDATE configuration SET is_active = 1 WHERE id = :id");
    if (!ok)
    {
        QString msg = QString("FAILED preparing query for active configuration setup: %1")
                .arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    query.bindValue(":id", id);
    if (!query.exec())
    {
        QString msg = QString("FAILED setting of new active configuration: %1")
                .arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    updateActiveConfigField();
}

//---------------------------------------------------------------------------------------
void ConfigsGroupBox::updateActiveConfigField()
{
    loggable.logMessage(objectName(), QtDebugMsg, "Update active configuration field...");

    QSqlQuery query("SELECT * FROM configuration WHERE is_active = 1 LIMIT 1");

    if (!query.exec())
    {
        QString msg = QString("FAILED loading active configuration: %1").arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return;
    }

    if (query.first())
    {
        ui->activeConfigLabel->setText(query.value("name").toString());
    }
    else
    {
        loggable.logMessage(objectName(), QtWarningMsg, "ERROR: active configuration not found!");
        ui->activeConfigLabel->clear();
        return;
    }
}

//---------------------------------------------------------------------------------------
void ConfigsGroupBox::updateData()
{
    updateActiveConfigField();
    model->select();
    while(model->canFetchMore())
        model->fetchMore();
}

//---------------------------------------------------------------------------------------
void ConfigsGroupBox::init()
{
    model = new QSqlTableModel(this);
    model->setTable("configuration");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);

    int idFieldNo = model->fieldIndex("id");
    int nameFieldNo = model->fieldIndex("name");
    int isActiveFieldNo = model->fieldIndex("is_active");

    model->setHeaderData(nameFieldNo, Qt::Horizontal, tr("Name"));

    ui->tableView->setModel(model);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->hideColumn(idFieldNo);
    ui->tableView->hideColumn(isActiveFieldNo);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    connect(ui->addButton, &QPushButton::clicked, this, &ConfigsGroupBox::addConfig);
    connect(ui->deleteButton, &QPushButton::clicked, this, &ConfigsGroupBox::deleteConfig);
    connect(ui->activateButton, &QPushButton::clicked, this, &ConfigsGroupBox::activateConfig);

    updateData();
}

//---------------------------------------------------------------------------------------
