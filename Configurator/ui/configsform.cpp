#include "configsform.h"
#include "ui_configsform.h"

//---------------------------------------------------------------------------------------
ConfigsForm::ConfigsForm(TreeModel *treeModel, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigsForm)
{
    ui->setupUi(this);
    setObjectName(tr("Configurations"));

    ConfigsProxyModel *proxyModel = new ConfigsProxyModel(this);

}

//---------------------------------------------------------------------------------------
ConfigsForm::~ConfigsForm()
{
    delete ui;
}

//---------------------------------------------------------------------------------------
