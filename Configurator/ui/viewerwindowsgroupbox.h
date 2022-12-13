#ifndef VIEWERWINDOWSGROUPBOX_H
#define VIEWERWINDOWSGROUPBOX_H

#include <QGroupBox>

#include <QSqlRelationalTableModel>

#include "loggable.h"

namespace Ui {
class ViewerWindowsGroupBox;
}

class ViewerWindowsGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit ViewerWindowsGroupBox(QWidget *parent = nullptr);
    ~ViewerWindowsGroupBox();

public slots:
    void addWindow();
    void editWindow();
    void deleteWindow();

    void updateTableView();

private:
    void init();

    Ui::ViewerWindowsGroupBox *ui;
    Loggable loggable;

    QSqlRelationalTableModel *model;

};

#endif // VIEWERWINDOWSGROUPBOX_H
