#ifndef EDITSERVICEDIALOG_H
#define EDITSERVICEDIALOG_H

#include <QDialog>
#include <QDataWidgetMapper>
#include <QSqlRelationalTableModel>

#include "loggable.h"

namespace Ui {
class EditServiceDialog;
}

class EditServiceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditServiceDialog(std::optional<int> id, QWidget *parent = nullptr);
    ~EditServiceDialog();

public slots:
    bool checkAndSubmit();
    void accept() override;

signals:
    void modelUpdated();

private:
    void init();
    void fillExistingDataFields();
    void createNewRecord();

    Ui::EditServiceDialog *ui;
    Loggable loggable;
    QSqlRelationalTableModel *model;
    QDataWidgetMapper *mapper;

    std::optional<int> serviceId;
};

#endif // EDITSERVICEDIALOG_H
