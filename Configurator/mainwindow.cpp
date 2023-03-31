#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

#include <QListWidget>
#include <QStackedWidget>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "configtreeitem.h"
#include "configsform.h"


static const char* configDbFileName = "tvvm-config.db";

//---------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("TVVM Configurator (pre-pre-alpha)");

    ui->containerBox->setLayout(new QVBoxLayout());

    initDatabase();
    
    
    treeModel = new TreeModel(this);
    ui->treeView->setModel(treeModel);
    ui->treeView->header()->hide();
    for (int i = 1; i < treeModel->columnCount(); ++i)
        ui->treeView->hideColumn(i);
    ui->treeView->expandAll();

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::onTreeViewContextMenu);
    connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::onTreeViewSelectionChange);

    createContextMenusForTreeView();


//    setFixedHeight(sizeHint().height());
}

//---------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}

//---------------------------------------------------------------------------------------
void MainWindow::openSourcesDialog()
{

}

//---------------------------------------------------------------------------------------
void MainWindow::createContextMenusForTreeView()
{
//    createConfigContextMenu();
//    createServiceContextMenu();


}

//---------------------------------------------------------------------------------------
void MainWindow::onTreeViewContextMenu(const QPoint &point)
{
    QModelIndex index = ui->treeView->indexAt(point);
    if (!index.isValid())
        return;

    int row = index.row();
    QModelIndex parentIndex = treeModel->parent(index);

    QModelIndex idx = treeModel->index(row, TreeItem::Column::Name, parentIndex);

    TreeItem *item = treeModel->getItem(idx);
    int itemType = item->getType();

    QMenu menu;

    switch (itemType)
    {
    case TreeItem::Type::Config:
        if (!item->data(ConfigTreeItem::Column::IsActive).toBool())
        {
            auto makeActiveAction = new QAction(tr("Make active"), &menu);
            makeActiveAction->setData(idx);
            connect(makeActiveAction, &QAction::triggered, this, &MainWindow::makeConfigActive);
            menu.addAction(makeActiveAction);
        }
        auto removeConfigAction = new QAction(tr("Remove configuration"), &menu);
        connect(removeConfigAction, &QAction::triggered, this, &MainWindow::removeConfig);
        removeConfigAction->setData(idx);
        menu.addAction(removeConfigAction);
    }

    if (!menu.isEmpty())
        menu.exec(ui->treeView->viewport()->mapToGlobal(point));

}

//---------------------------------------------------------------------------------------
void MainWindow::onTreeViewSelectionChange(const QItemSelection &selected)
{
    clearContainerBox();

    QModelIndexList indexes = selected.indexes();
    if (indexes.empty())
        return;

    QModelIndex current = indexes.first();
    if (!current.isValid())
        return;

    TreeItem *item = treeModel->getItem(current);
    int itemType = item->getType();

    QWidget *newWidget = nullptr;
    switch (itemType)
    {
    case TreeItem::ConfigsFolder:
        newWidget = new ConfigsForm();
    }

    if (newWidget)
    {
        ui->containerBox->setTitle(newWidget->objectName());
        ui->containerBox->layout()->addWidget(newWidget);
    }
}

//---------------------------------------------------------------------------------------
void MainWindow::makeConfigActive()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    QModelIndex idx = action->data().toModelIndex();
    if (!idx.isValid())
        return;

    treeModel->makeConfigActive(idx);
}

//---------------------------------------------------------------------------------------
void MainWindow::removeConfig()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    QModelIndex idx = action->data().toModelIndex();
    if (!idx.isValid())
        return;

    TreeItem *item = treeModel->getItem(idx);
    QString configName = item->data(ConfigTreeItem::Column::Name).toString();

    int n = QMessageBox::warning(this, tr("Attention"),
                                 tr("Do you want delete configuration \"%1\"?\n"
                                    "All data associated with it will also be deleted!")
                                     .arg(configName),
                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(n == QMessageBox::No)
        return;

    treeModel->removeConfig(idx);
}

//---------------------------------------------------------------------------------------
void MainWindow::initDatabase()
{
    /// TODO: create new datebase if database file doesn't exist.
    QString appBase = QString::fromStdString(configDbFileName);

    loggable.logMessage(objectName(), QtDebugMsg, "Connect to the config database...");

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(appBase);
    if(!db.open())
    {
        loggable.logMessage(objectName(), QtCriticalMsg, "FAILED openning config database file!");
        return;
    }

    QSqlQuery query;
    if(!query.exec("PRAGMA foreign_keys = ON"))
    {
        QString msg = QString("SQL query ERROR!\n"
                              "- query: %1\n"
                              "- error: %2")
                .arg(query.lastQuery(), query.lastError().text());
        loggable.logMessage(objectName(), QtWarningMsg, msg);
    }
}

//---------------------------------------------------------------------------------------
void MainWindow::clearContainerBox()
{
    QLayoutItem *item;
    while ((item = ui->containerBox->layout()->takeAt(0)) != nullptr)
    {
        delete item->widget();
        delete item;
    }
    ui->containerBox->setTitle("");

}

//---------------------------------------------------------------------------------------
//void MainWindow::createConfigContextMenu()
//{
//    makeActiveConfigAction = new QAction(tr("Make active"), this);
//    removeConfigAction = new QAction(tr("Remove configuration"), this);

//    configContextMenu = new QMenu(this);

//    contextMenus[TreeItem::Config] = configContextMenu;
//}

//---------------------------------------------------------------------------------------
//void MainWindow::createServiceContextMenu()
//{
//    removeServiceAction = new QAction(tr("Delete service"), this);
//    disableServiceAction = new QAction(tr("Disable service"), this);

//    serviceContextMenu = new QMenu(this);
//    serviceContextMenu->addAction(disableServiceAction);
//    serviceContextMenu->addAction(removeServiceAction);

//    contextMenus[TreeItem::Config] = serviceContextMenu;
//}

//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
