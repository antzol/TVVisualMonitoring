#include "viewerwindowsgroupbox.h"
#include "ui_viewerwindowsgroupbox.h"

#include <QMessageBox>

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelationalDelegate>

#include "checkboxdelegate.h"
#include "configstructs.h"
#include "editviewerwindowdialog.h"

//---------------------------------------------------------------------------------------
ViewerWindowsGroupBox::ViewerWindowsGroupBox(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ViewerWindowsGroupBox)
{
    ui->setupUi(this);
    init();
}

//---------------------------------------------------------------------------------------
ViewerWindowsGroupBox::~ViewerWindowsGroupBox()
{
    delete ui;
}

//---------------------------------------------------------------------------------------
void ViewerWindowsGroupBox::addWindow()
{
    EditViewerWindowDialog dlg(std::nullopt);
    connect(&dlg, &EditViewerWindowDialog::modelUpdated, this, &ViewerWindowsGroupBox::updateTableView);
    dlg.exec();
}

//---------------------------------------------------------------------------------------
void ViewerWindowsGroupBox::editWindow()
{
    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedIndexes();

    int size = indexes.size();

    if(!size)
    {
        QMessageBox::warning(this, tr("Warning"), tr("You must select one viewer for editing!"));
        return;
    }

    int idFieldNo = model->fieldIndex("id");
    int row = indexes.first().row();
    int id = model->data(model->index(row, idFieldNo)).toInt();

    EditViewerWindowDialog dlg(id);
    connect(&dlg, &EditViewerWindowDialog::modelUpdated, this, &ViewerWindowsGroupBox::updateTableView);
    dlg.exec();
}

//---------------------------------------------------------------------------------------
void ViewerWindowsGroupBox::deleteWindow()
{
    loggable.logMessage(objectName(), QtDebugMsg, "Delete viewer...");

    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();

    int size = indexes.size();

    if(size)
    {
        int n = QMessageBox::warning(this, tr("Attention"),
                                     tr("Do you want delete %1 viewer(s) from the database?\n"
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
void ViewerWindowsGroupBox::updateTableView()
{
    model->select();
    while(model->canFetchMore())
        model->fetchMore();
}

//---------------------------------------------------------------------------------------
void ViewerWindowsGroupBox::init()
{
    model = new QSqlRelationalTableModel(this);
    model->setTable("mosaic_viewer");

    int idFieldNo = model->fieldIndex("id");
    int nameFieldNo = model->fieldIndex("name");
    int configFieldNo = model->fieldIndex("configuration");
    int widthFieldNo = model->fieldIndex("width");
    int heightFieldNo = model->fieldIndex("height");
    int monitorFieldNo = model->fieldIndex("monitor");

    model->setRelation(configFieldNo, QSqlRelation("configuration", "id", "name"));

    model->setHeaderData(idFieldNo, Qt::Horizontal, tr("ID"));
    model->setHeaderData(nameFieldNo, Qt::Horizontal, tr("Name"));
    model->setHeaderData(configFieldNo, Qt::Horizontal, tr("Configuration"));
    model->setHeaderData(widthFieldNo, Qt::Horizontal, tr("Width"));
    model->setHeaderData(heightFieldNo, Qt::Horizontal, tr("Height"));
    model->setHeaderData(monitorFieldNo, Qt::Horizontal, tr("Monitor"));

    ui->tableView->setModel(model);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->hideColumn(idFieldNo);
    ui->tableView->setColumnWidth(nameFieldNo, 170);
    ui->tableView->setItemDelegate(new QSqlRelationalDelegate(ui->tableView));
    ui->tableView->horizontalHeader()->setStretchLastSection(true);


    connect(ui->addButton, &QPushButton::clicked, this, &ViewerWindowsGroupBox::addWindow);
    connect(ui->deleteButton, &QPushButton::clicked, this, &ViewerWindowsGroupBox::deleteWindow);
    connect(ui->editButton, &QPushButton::clicked, this, &ViewerWindowsGroupBox::editWindow);

    updateTableView();
}

//---------------------------------------------------------------------------------------
