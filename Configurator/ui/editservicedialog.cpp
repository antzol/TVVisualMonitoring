#include "editservicedialog.h"
#include "ui_editservicedialog.h"

#include <QMessageBox>

#include <QSqlError>
#include <QSqlRecord>
#include <QSqlRelationalDelegate>

//---------------------------------------------------------------------------------------
EditServiceDialog::EditServiceDialog(std::optional<int> id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditServiceDialog),
    serviceId(id)
{
    ui->setupUi(this);
    init();

    if (serviceId)
    {
        fillExistingDataFields();
        setWindowTitle(tr("Edit service"));
    }
    else
    {
        createNewRecord();
        setWindowTitle(tr("Add service"));
    }

    connect(ui->applyButton, &QPushButton::clicked, this, &EditServiceDialog::checkAndSubmit);
    connect(ui->okButton, &QPushButton::clicked, this, &EditServiceDialog::accept);
    connect(ui->cancelButton, &QPushButton::clicked, this, &EditServiceDialog::reject);

}

//---------------------------------------------------------------------------------------
EditServiceDialog::~EditServiceDialog()
{
    delete ui;
}

//---------------------------------------------------------------------------------------
bool EditServiceDialog::checkAndSubmit()
{
    if (ui->nameLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("You must enter source name!"), QMessageBox::Close);
        return false;
    }
    else if (ui->sourceComboBox->currentIndex() == -1)
    {
        QMessageBox::warning(this, tr("Warning"), tr("You must select source!"), QMessageBox::Close);
        return false;
    }
    else if (ui->typeComboBox->currentIndex() == -1)
    {
        QMessageBox::warning(this, tr("Warning"), tr("You must select service type!"), QMessageBox::Close);
        return false;
    }

    if (!mapper->submit())
    {
        QString msg = QString("FAILED submitting of service data: %1").arg(model->lastError().text());
        loggable.logMessage(objectName(), QtCriticalMsg, msg);
        return false;
    }

    emit modelUpdated();

    return true;
}

//---------------------------------------------------------------------------------------
void EditServiceDialog::accept()
{
    if (checkAndSubmit())
        QDialog::accept();
}

//---------------------------------------------------------------------------------------
void EditServiceDialog::init()
{
    model = new QSqlRelationalTableModel(this);
    model->setTable("service");

    int nameFieldNo = model->fieldIndex("name");
    int typeFieldNo = model->fieldIndex("type");
    int sourceFieldNo = model->fieldIndex("source");
    int sidFieldNo = model->fieldIndex("sid");
    int enabledFieldNo = model->fieldIndex("enabled");

    model->setRelation(sourceFieldNo, QSqlRelation("source", "id", "name"));
    model->setRelation(typeFieldNo, QSqlRelation("service_type", "id", "name"));

    QSqlTableModel *sourceModel = model->relationModel(sourceFieldNo);
    sourceModel->setSort(sourceModel->fieldIndex("name"), Qt::AscendingOrder);
    ui->sourceComboBox->setModel(sourceModel);
    ui->sourceComboBox->setModelColumn(sourceModel->fieldIndex("name"));
    sourceModel->select();

    QSqlTableModel *typeModel = model->relationModel(typeFieldNo);
    typeModel->setSort(sourceModel->fieldIndex("name"), Qt::AscendingOrder);
    ui->typeComboBox->setModel(typeModel);
    ui->typeComboBox->setModelColumn(typeModel->fieldIndex("name"));
    typeModel->select();

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    mapper->setModel(model);
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    mapper->addMapping(ui->nameLineEdit, nameFieldNo);
    mapper->addMapping(ui->sourceComboBox, sourceFieldNo);
    mapper->addMapping(ui->typeComboBox, typeFieldNo);
    mapper->addMapping(ui->sidSpinBox, sidFieldNo);
    mapper->addMapping(ui->enabledCheckBox, enabledFieldNo);

    model->select();
}

//---------------------------------------------------------------------------------------
void EditServiceDialog::fillExistingDataFields()
{
    int idFieldNo = model->fieldIndex("id");
    for(int row = 0; row < model->rowCount(); ++row)
    {
        QSqlRecord record = model->record(row);
        if(record.value(idFieldNo).toInt() == serviceId.value())
        {
            mapper->setCurrentIndex(row);
            break;
        }
    }
}

//---------------------------------------------------------------------------------------
void EditServiceDialog::createNewRecord()
{
    model->insertRow(0);
    mapper->setCurrentIndex(0);

    /// FIXME: add support for radio services
    int cnt = ui->typeComboBox->count();
    for (int i = 0; i < cnt; ++i)
    {
        if (ui->typeComboBox->itemText(i) == "TV")
        {
            ui->typeComboBox->setCurrentIndex(i);
            break;
        }
    }
    ui->sidSpinBox->setValue(1010);
}

//---------------------------------------------------------------------------------------
