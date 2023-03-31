#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <unordered_map>

#include "loggable.h"
#include "qitemselectionmodel.h"
#include "treemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void openSourcesDialog();
    void createContextMenusForTreeView();

    void onTreeViewContextMenu(const QPoint &point);
    void onTreeViewSelectionChange(const QItemSelection &selected);

    void makeConfigActive();
    void removeConfig();



private:
    void initDatabase();
    void clearContainerBox();
//    void createConfigContextMenu();
//    void createServiceContextMenu();


    Ui::MainWindow *ui;
    Loggable loggable;

    TreeModel *treeModel;


//    std::unordered_map<TreeItem::Type, QMenu*> contextMenus;

    QMenu *configContextMenu;
    QAction *makeActiveConfigAction;
    QAction *removeConfigAction;


    QMenu *serviceContextMenu;
    QAction *removeServiceAction;
    QAction *disableServiceAction;


};

#endif // MAINWINDOW_H
