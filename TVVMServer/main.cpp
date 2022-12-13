#include "mainwindow.h"

#include <QApplication>

#include "logger.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Logger *logger = Logger::getInstance();
    logger->writeMessage("App", QtInfoMsg, "Start application...");

    MainWindow w;
    w.show();
    return a.exec();
}
