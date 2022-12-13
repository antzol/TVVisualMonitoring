#include "editsourcedialog.h"
#include "ui_editsourcedialog.h"

#include <QMessageBox>

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelationalDelegate>

//---------------------------------------------------------------------------------------
EditSourceDialog::EditSourceDialog(std::optional<int> id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditSourceDialog),
    sourceId(id)
{
    ui->setupUi(this);

    configureMainUi();
    configureUdpUi();

    if (sourceId)
    {
        fillExistingDataFields();
        setWindowTitle(tr("Edit source"));
    }
    else
    {
        createNewRecord();
        setWindowTitle(tr("Add source"));
    }

    connect(ui->applyButton, &QPushButton::clicked, this, &EditSourceDialog::checkAndSubmit);
    connect(ui->okButton, &QPushButton::clicked, this, &EditSourceDialog::accept);
    connect(ui->cancelButton, &QPushButton::clicked, this, &EditSourceDialog::reject);
}

//---------------------------------------------------------------------------------------
EditSourceDialog::~EditSourceDialog()
{
    delete ui;
}

//---------------------------------------------------------------------------------------
bool EditSourceDialog::checkAndSubmit()
{
    if (ui->nameLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("You must enter source name!"), QMessageBox::Close);
        return false;
    }
    else if (ui->configComboBox->currentIndex() == -1)
    {
        QMessageBox::warning(this, tr("Warning"), tr("You must select configuration!"), QMessageBox::Close);
        return false;
    }
    else if (ui->udpMulticastAddressLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("You must enter multicast address!"), QMessageBox::Close);
        return false;
    }

    if (!sourceMapper->submit())
    {
        QString msg = QString("FAILED submitting of source data: %1").arg(sourceModel->lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return false;
    }

    QModelIndex idx = sourceModel->index(sourceMapper->currentIndex(), sourceModel->fieldIndex("id"));
    sourceId = sourceModel->data(idx).toInt();

    QSqlQuery query;

    bool ok = query.prepare("INSERT OR REPLACE INTO udp_source "
                            "(address, port, local_address, buffer_size, source, overrun_nonfatal) "
                            "VALUES "
                            "(:addr, :port, :loc_addr, :buf_size, :source, :overrun_nonfatal)");

    if (!ok)
    {
        QString msg = QString("FAILED SQL query preparing for UDP data submitting: %1")
                .arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return false;
    }

    query.bindValue(":addr", ui->udpMulticastAddressLineEdit->text());
    query.bindValue(":port", ui->udpPortSpinBox->value());
    query.bindValue(":loc_addr", ui->udpLocalAddressLineEdit->text());
    query.bindValue(":buf_size", ui->udpBufferSizeSpinBox->value());
    query.bindValue(":source", sourceId.value());
    query.bindValue(":overrun_nonfatal", ui->udpOverrunNonFatalCheckBox->isChecked() ? 1 : 0);


    if (!query.exec())
    {
        QString msg = QString("FAILED SQL query executing for UDP data submitting: %1")
                .arg(query.lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return false;
    }

    emit modelUpdated();

    return true;
}

//---------------------------------------------------------------------------------------
void EditSourceDialog::accept()
{
    if (checkAndSubmit())
        QDialog::accept();
}

//---------------------------------------------------------------------------------------
void EditSourceDialog::configureMainUi()
{
    sourceModel = new QSqlRelationalTableModel(this);
    sourceModel->setTable("source");

    int nameFieldNo = sourceModel->fieldIndex("name");
    int typeFieldNo = sourceModel->fieldIndex("type");
    int configFieldNo = sourceModel->fieldIndex("configuration");
    int autoRestartFieldNo = sourceModel->fieldIndex("auto_restart_enabled");
    int restartIntervalFieldNo = sourceModel->fieldIndex("auto_restart_interval");

    sourceModel->setRelation(configFieldNo, QSqlRelation("configuration", "id", "name"));
    sourceModel->setRelation(typeFieldNo, QSqlRelation("source_type", "id", "name"));

    QSqlTableModel *configModel = sourceModel->relationModel(configFieldNo);
    configModel->setSort(configModel->fieldIndex("name"), Qt::AscendingOrder);
    ui->configComboBox->setModel(configModel);
    ui->configComboBox->setModelColumn(configModel->fieldIndex("name"));
    configModel->select();

    QSqlTableModel *typeModel = sourceModel->relationModel(typeFieldNo);
    typeModel->setSort(configModel->fieldIndex("name"), Qt::AscendingOrder);
    ui->typeComboBox->setModel(typeModel);
    ui->typeComboBox->setModelColumn(typeModel->fieldIndex("name"));
    typeModel->select();

    sourceMapper = new QDataWidgetMapper(this);
    sourceMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    sourceMapper->setModel(sourceModel);
    sourceMapper->setItemDelegate(new QSqlRelationalDelegate(this));
    sourceMapper->addMapping(ui->nameLineEdit, nameFieldNo);
    sourceMapper->addMapping(ui->configComboBox, configFieldNo);
    sourceMapper->addMapping(ui->typeComboBox, typeFieldNo);
    sourceMapper->addMapping(ui->enableAutoRestartCheckBox, autoRestartFieldNo);
    sourceMapper->addMapping(ui->restartIntervalSpinBox, restartIntervalFieldNo);

    sourceModel->select();
}

//---------------------------------------------------------------------------------------
void EditSourceDialog::configureUdpUi()
{
    udpModel = new QSqlTableModel();
    udpModel->setTable("udp_source");

    int addressFieldNo = udpModel->fieldIndex("address");
    int portFieldNo = udpModel->fieldIndex("port");
    int localAddressFieldNo = udpModel->fieldIndex("local_address");
    int bufferSizeFieldNo = udpModel->fieldIndex("buffer_size");
    int overrunNonFatalFieldNo = udpModel->fieldIndex("overrun_nonfatal");

    udpMapper = new QDataWidgetMapper(this);
    udpMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    udpMapper->setModel(udpModel);
    udpMapper->addMapping(ui->udpMulticastAddressLineEdit, addressFieldNo);
    udpMapper->addMapping(ui->udpPortSpinBox, portFieldNo);
    udpMapper->addMapping(ui->udpLocalAddressLineEdit, localAddressFieldNo);
    udpMapper->addMapping(ui->udpBufferSizeSpinBox, bufferSizeFieldNo);
    udpMapper->addMapping(ui->udpOverrunNonFatalCheckBox, overrunNonFatalFieldNo);

    udpModel->select();
}

//---------------------------------------------------------------------------------------
void EditSourceDialog::fillExistingDataFields()
{
    int idFieldNo = sourceModel->fieldIndex("id");
    for(int row = 0; row < sourceModel->rowCount(); ++row)
    {
        QSqlRecord record = sourceModel->record(row);
        if(record.value(idFieldNo).toInt() == sourceId.value())
        {
            sourceMapper->setCurrentIndex(row);
            break;
        }
    }

    int srcKeyForUdpFieldNo = udpModel->fieldIndex("source");
    std::optional<int> udpRow;
    for(int row = 0; row < udpModel->rowCount(); ++row)
    {
        QSqlRecord record = udpModel->record(row);
        if(record.value(srcKeyForUdpFieldNo).toInt() == sourceId.value())
        {
            udpMapper->setCurrentIndex(row);
            udpRow = row;
            break;
        }
    }
    if (!udpRow)
    {
        udpModel->insertRow(0);
        udpMapper->setCurrentIndex(0);
    }
}

//---------------------------------------------------------------------------------------
void EditSourceDialog::createNewRecord()
{
    sourceModel->insertRow(0);
    sourceMapper->setCurrentIndex(0);

    udpModel->insertRow(0);
    udpMapper->setCurrentIndex(0);

    /// FIXME: add support for other types of sources
    int cnt = ui->typeComboBox->count();
    for (int i = 0; i < cnt; ++i)
    {
        if (ui->typeComboBox->itemText(i) == "UDP")
        {
            ui->typeComboBox->setCurrentIndex(i);
            break;
        }
    }
    ui->configComboBox->setCurrentIndex(-1);
}

//---------------------------------------------------------------------------------------
