#ifndef SERVICESGROUPBOX_H
#define SERVICESGROUPBOX_H

#include <QGroupBox>

#include <QSqlRelationalTableModel>

#include "loggable.h"

namespace Ui {
class ServicesGroupBox;
}

class ServicesGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit ServicesGroupBox(QWidget *parent = nullptr);
    ~ServicesGroupBox();

public slots:
    void addService();
    void editService();
    void deleteService();

    void updateTableView();

private:
    void init();

    Ui::ServicesGroupBox *ui;
    Loggable loggable;

    QSqlRelationalTableModel *model;
};

#endif // SERVICESGROUPBOX_H
