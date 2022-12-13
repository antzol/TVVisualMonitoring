#include "sourcesgroupbox.h"
#include "ui_sourcesgroupbox.h"

#include <QMessageBox>

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelationalDelegate>

#include "checkboxdelegate.h"
#include "configstructs.h"
#include "editsourcedialog.h"

//---------------------------------------------------------------------------------------
SourcesGroupBox::SourcesGroupBox(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::SourcesGroupBox)
{
    ui->setupUi(this);
    init();
}

//---------------------------------------------------------------------------------------
SourcesGroupBox::~SourcesGroupBox()
{
    delete ui;
}

//---------------------------------------------------------------------------------------
void SourcesGroupBox::addSource()
{
    EditSourceDialog dlg(std::nullopt);
    connect(&dlg, &EditSourceDialog::modelUpdated, this, &SourcesGroupBox::updateTableView);
    dlg.exec();
}

//---------------------------------------------------------------------------------------
void SourcesGroupBox::editSource()
{
    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedIndexes();

    int size = indexes.size();

    if(!size)
    {
        QMessageBox::warning(this, tr("Warning"), tr("You must select one source for editing!"));
        return;
    }

    int idFieldNo = model->fieldIndex("id");
    int row = indexes.first().row();
    int id = model->data(model->index(row, idFieldNo)).toInt();

    EditSourceDialog dlg(id);
    connect(&dlg, &EditSourceDialog::modelUpdated, this, &SourcesGroupBox::updateTableView);
    dlg.exec();
}

//---------------------------------------------------------------------------------------
void SourcesGroupBox::deleteSource()
{
    loggable.logMessage(objectName(), QtDebugMsg, "Delete media source...");

    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();

    int size = indexes.size();

    if(size)
    {
        int n = QMessageBox::warning(this, tr("Attention"),
                                     tr("Do you want delete %1 source(s) from the database?\n"
                                        "All data associated with them will also be deleted!")
                                     .arg(size),
                                     QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if(n == QMessageBox::No)
            return;
    }

    for (auto& idx : indexes)
        model->removeRows(idx.row(), 1);

    updateTableView();
}

//---------------------------------------------------------------------------------------
void SourcesGroupBox::updateTableView()
{
    model->select();
    while(model->canFetchMore())
        model->fetchMore();
}

//---------------------------------------------------------------------------------------
void SourcesGroupBox::init()
{
    model = new QSqlRelationalTableModel(this);
    model->setTable("source");

    int idFieldNo = model->fieldIndex("id");
    int nameFieldNo = model->fieldIndex("name");
    int typeFieldNo = model->fieldIndex("type");
    int configFieldNo = model->fieldIndex("configuration");
    int autoRestartFieldNo = model->fieldIndex("auto_restart_enabled");
    int restartIntervalFieldNo = model->fieldIndex("auto_restart_interval");

    model->setRelation(typeFieldNo, QSqlRelation("source_type", "id", "name"));
    model->setRelation(configFieldNo, QSqlRelation("configuration", "id", "name"));

    model->setHeaderData(idFieldNo, Qt::Horizontal, tr("ID"));
    model->setHeaderData(nameFieldNo, Qt::Horizontal, tr("Name"));
    model->setHeaderData(typeFieldNo, Qt::Horizontal, tr("Type"));
    model->setHeaderData(configFieldNo, Qt::Horizontal, tr("Configuration"));
    model->setHeaderData(autoRestartFieldNo, Qt::Horizontal, tr("Auto restart"));
    model->setHeaderData(restartIntervalFieldNo, Qt::Horizontal, tr("Restart interval"));

    ui->tableView->setModel(model);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->hideColumn(idFieldNo);
    ui->tableView->setColumnWidth(nameFieldNo, 170);
//    ui->tableView->resizeColumnToContents(nameFieldNo);
    ui->tableView->setItemDelegate(new QSqlRelationalDelegate(ui->tableView));
//    ui->tableView->setItemDelegateForColumn(autoRestartFieldNo, new CheckBoxDelegate(ui->tableView));
    ui->tableView->horizontalHeader()->setStretchLastSection(true);


    connect(ui->addButton, &QPushButton::clicked, this, &SourcesGroupBox::addSource);
    connect(ui->deleteButton, &QPushButton::clicked, this, &SourcesGroupBox::deleteSource);
    connect(ui->editButton, &QPushButton::clicked, this, &SourcesGroupBox::editSource);

    updateTableView();
}

//---------------------------------------------------------------------------------------
