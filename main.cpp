#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    qint64 pid = QCoreApplication::applicationPid();
    // Đặt tiêu đề cửa sổ là PID của process
    w.setWindowTitle("PID: " + QString::number(pid));
    w.show();
    return a.exec();
}
