#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "loggable.h"

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



private:
    void initDatabase();


    Ui::MainWindow *ui;
    Loggable loggable;


};
#endif // MAINWINDOW_H
