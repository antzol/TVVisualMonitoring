#include "servicesgroupbox.h"
#include "ui_servicesgroupbox.h"

#include <QMessageBox>

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelationalDelegate>

#include "checkboxdelegate.h"
#include "configstructs.h"
#include "editservicedialog.h"

//---------------------------------------------------------------------------------------
ServicesGroupBox::ServicesGroupBox(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ServicesGroupBox)
{
    ui->setupUi(this);
    init();
}

//---------------------------------------------------------------------------------------
ServicesGroupBox::~ServicesGroupBox()
{
    delete ui;
}

//---------------------------------------------------------------------------------------
void ServicesGroupBox::addService()
{
    EditServiceDialog dlg(std::nullopt);
    connect(&dlg, &EditServiceDialog::modelUpdated, this, &ServicesGroupBox::updateTableView);
    dlg.exec();
}

//---------------------------------------------------------------------------------------
void ServicesGroupBox::editService()
{
    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedIndexes();

    int size = indexes.size();

    if(!size)
    {
        QMessageBox::warning(this, tr("Warning"), tr("You must select one service for editing!"));
        return;
    }

    int idFieldNo = model->fieldIndex("id");
    int row = indexes.first().row();
    int id = model->data(model->index(row, idFieldNo)).toInt();

    EditServiceDialog dlg(id);
    connect(&dlg, &EditServiceDialog::modelUpdated, this, &ServicesGroupBox::updateTableView);
    dlg.exec();
}

//---------------------------------------------------------------------------------------
void ServicesGroupBox::deleteService()
{
    loggable.logMessage(objectName(), QtDebugMsg, "Delete media source...");

    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();

    int size = indexes.size();

    if(size)
    {
        int n = QMessageBox::warning(this, tr("Attention"),
                                     tr("Do you want delete %1 service(s) from the database?\n"
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
void ServicesGroupBox::updateTableView()
{
    model->select();
    while(model->canFetchMore())
        model->fetchMore();
}

//---------------------------------------------------------------------------------------
void ServicesGroupBox::init()
{
    model = new QSqlRelationalTableModel(this);
    model->setTable("service");

    int idFieldNo = model->fieldIndex("id");
    int nameFieldNo = model->fieldIndex("name");
    int typeFieldNo = model->fieldIndex("type");
    int sidFieldNo = model->fieldIndex("sid");
    int sourceFieldNo = model->fieldIndex("source");
    int enabledFieldNo = model->fieldIndex("enabled");

    model->setRelation(typeFieldNo, QSqlRelation("service_type", "id", "name"));
    model->setRelation(sourceFieldNo, QSqlRelation("source", "id", "name"));

    model->setHeaderData(idFieldNo, Qt::Horizontal, tr("ID"));
    model->setHeaderData(nameFieldNo, Qt::Horizontal, tr("Name"));
    model->setHeaderData(typeFieldNo, Qt::Horizontal, tr("Type"));
    model->setHeaderData(sidFieldNo, Qt::Horizontal, tr("SID"));
    model->setHeaderData(sourceFieldNo, Qt::Horizontal, tr("Source"));
    model->setHeaderData(enabledFieldNo, Qt::Horizontal, tr("Enabled"));

    ui->tableView->setModel(model);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->hideColumn(idFieldNo);
    ui->tableView->setColumnWidth(nameFieldNo, 170);
//    ui->tableView->resizeColumnToContents(nameFieldNo);
    ui->tableView->setItemDelegate(new QSqlRelationalDelegate(ui->tableView));
//    ui->tableView->setItemDelegateForColumn(autoRestartFieldNo, new CheckBoxDelegate(ui->tableView));
    ui->tableView->horizontalHeader()->setStretchLastSection(true);


    connect(ui->addButton, &QPushButton::clicked, this, &ServicesGroupBox::addService);
    connect(ui->deleteButton, &QPushButton::clicked, this, &ServicesGroupBox::deleteService);
    connect(ui->editButton, &QPushButton::clicked, this, &ServicesGroupBox::editService);

    updateTableView();
}

//---------------------------------------------------------------------------------------
