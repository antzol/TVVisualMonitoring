#ifndef SOURCESGROUPBOX_H
#define SOURCESGROUPBOX_H

#include <QGroupBox>

#include <QSqlRelationalTableModel>

#include "loggable.h"

namespace Ui {
class SourcesGroupBox;
}

class SourcesGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit SourcesGroupBox(QWidget *parent = nullptr);
    ~SourcesGroupBox();

public slots:
    void addSource();
    void editSource();
    void deleteSource();

    void updateTableView();

private:
    void init();

    Ui::SourcesGroupBox *ui;
    Loggable loggable;

    QSqlRelationalTableModel *model;
};

#endif // SOURCESGROUPBOX_H
