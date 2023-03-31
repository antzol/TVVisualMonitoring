#ifndef CONFIGSFORM_H
#define CONFIGSFORM_H

#include <QWidget>

#include "treemodel.h"
#include "configsproxymodel.h"

namespace Ui {
class ConfigsForm;
}

class ConfigsForm : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigsForm(TreeModel *treeModel, QWidget *parent = nullptr);
    ~ConfigsForm();

private:
    Ui::ConfigsForm *ui;
};

#endif // CONFIGSFORM_H
