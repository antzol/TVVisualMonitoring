#ifndef CONFIGSGROUPBOX_H
#define CONFIGSGROUPBOX_H

#include <QGroupBox>

#include <QSqlTableModel>

#include "loggable.h"

namespace Ui {
class ConfigsGroupBox;
}

class ConfigsGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit ConfigsGroupBox(QWidget *parent = nullptr);
    ~ConfigsGroupBox();

public slots:
    void addConfig();
    void deleteConfig();
    void activateConfig();
    void updateActiveConfigField();

    void updateData();

private:
    void init();

    Ui::ConfigsGroupBox *ui;
    Loggable loggable;

    QSqlTableModel *model;
};

#endif // CONFIGSGROUPBOX_H
