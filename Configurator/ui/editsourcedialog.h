#ifndef EDITSOURCEDIALOG_H
#define EDITSOURCEDIALOG_H

#include <QDialog>
#include <QDataWidgetMapper>
#include <QSqlRelationalTableModel>

#include "loggable.h"

namespace Ui {
class EditSourceDialog;
}

class EditSourceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditSourceDialog(std::optional<int> id, QWidget *parent = nullptr);
    ~EditSourceDialog();

public slots:
    bool checkAndSubmit();
    void accept() override;

signals:
    void modelUpdated();

private:
    void configureMainUi();
    void configureUdpUi();

    void fillExistingDataFields();
    void createNewRecord();

    Ui::EditSourceDialog *ui;
    Loggable loggable;
    QSqlRelationalTableModel *sourceModel;
    QDataWidgetMapper *sourceMapper;

    QSqlTableModel *udpModel;
    QDataWidgetMapper *udpMapper;

    std::optional<int> sourceId;

};

#endif // EDITSOURCEDIALOG_H
