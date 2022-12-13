#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "configsgroupbox.h"
#include "sourcesgroupbox.h"
#include "servicesgroupbox.h"
#include "viewerwindowsgroupbox.h"

static const char* configDbFileName = "tvvm-config.db";

//---------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("TVVM Configurator (pre-pre-alpha)");

    QStringList categories;
    categories << tr("Configurations") << tr("Sources") << tr("Services") << tr("Viewer Windows");
    ui->categoriesListWidget->addItems(categories);

    connect(ui->categoriesListWidget, &QListWidget::currentRowChanged,
            ui->stackedWidget, &QStackedWidget::setCurrentIndex);

    initDatabase();


    ConfigsGroupBox *configsBox = new ConfigsGroupBox();
    ui->configsPage->setLayout(new QVBoxLayout());
    ui->configsPage->layout()->addWidget(configsBox);

    SourcesGroupBox *sourcesBox = new SourcesGroupBox();
    ui->sourcesPage->setLayout(new QVBoxLayout());
    ui->sourcesPage->layout()->addWidget(sourcesBox);

    ServicesGroupBox *servicesBox = new ServicesGroupBox();
    ui->servicesPage->setLayout(new QVBoxLayout());
    ui->servicesPage->layout()->addWidget(servicesBox);

    ViewerWindowsGroupBox *viewerWindowsBox = new ViewerWindowsGroupBox();
    ui->viewerWindowsPage->setLayout(new QVBoxLayout());
    ui->viewerWindowsPage->layout()->addWidget(viewerWindowsBox);

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


//---------------------------------------------------------------------------------------
void MainWindow::initDatabase()
{
    /// TODO: create new datebase if database file doesn't exist.
    QString appFolder = QApplication::applicationDirPath() + "/";
    QString appBase = appFolder + QString::fromStdString(configDbFileName);

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


//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------
