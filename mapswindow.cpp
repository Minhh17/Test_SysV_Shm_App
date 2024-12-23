#include "mapswindow.h"
#include <QFile>
#include <QTextStream>
#include <QHeaderView>
#include <QCoreApplication>
#include <QDebug>

MapsWindow::MapsWindow(QWidget *parent)
    : QWidget(nullptr, Qt::Window) {
    // Bố cục chính
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Bảng hiển thị thông tin
    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(7); // 7 cột: Địa chỉ, Quyền, Kích thước, Offset, Thiết bị, Inode, File
    tableWidget->setHorizontalHeaderLabels({"Địa chỉ", "Quyền", "Kích thước", "Offset", "Thiết bị", "Inode", "File"});

    // Căn chỉnh kích thước cột
    tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // Địa chỉ
    tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // Quyền
    tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents); // Kích thước
    tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents); // Offset
    tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents); // Thiết bị
    tableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents); // Inode
    tableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);          // File

    // Bật tính năng sắp xếp bảng
    tableWidget->setSortingEnabled(true);

    // Tùy chỉnh giao diện bảng
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // Tắt chỉnh sửa trực tiếp

    layout->addWidget(tableWidget);

    // Thiết lập giao diện cửa sổ
    setLayout(layout);
    setWindowTitle("Thông tin trong Memory Maps của Shared Memory");
    resize(1200, 400);
}

void MapsWindow::loadMapsData() {
    QString pid = QString::number(QCoreApplication::applicationPid());
    QString command = "cat /proc/" + pid + "/maps | grep /SYSV";

    QProcess *process = new QProcess(this);
    connect(process, &QProcess::readyReadStandardOutput, [=]() {
        QStringList lines = QString::fromUtf8(process->readAllStandardOutput()).split("\n", Qt::SkipEmptyParts);
        tableWidget->setRowCount(0);

        for (const QString &line : lines) {
            QStringList parts = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() < 6) continue;

            QStringList addressParts = parts[0].split("-");
            if (addressParts.size() != 2) continue;

            QString startAddress = addressParts[0];
            QString endAddress = addressParts[1];
            QString permissions = parts[1];
            QString offset = parts[2];
            QString device = parts[3];
            QString inode = parts[4];
            QString fileName = line.mid(line.indexOf("/SYSV")).trimmed();

            // Tính kích thước
            bool okStart, okEnd;
            quint64 start = startAddress.toULongLong(&okStart, 16);
            quint64 end = endAddress.toULongLong(&okEnd, 16);
            QString size = okStart && okEnd ? QString::number(end - start) + " bytes" : "Unknown";

            // Thêm dòng vào bảng
            tableWidget->insertRow(tableWidget->rowCount());
            tableWidget->setItem(tableWidget->rowCount() - 1, 0, new QTableWidgetItem(startAddress + " - " + endAddress));
            tableWidget->setItem(tableWidget->rowCount() - 1, 1, new QTableWidgetItem(permissions));
            tableWidget->setItem(tableWidget->rowCount() - 1, 2, new QTableWidgetItem(size));
            tableWidget->setItem(tableWidget->rowCount() - 1, 3, new QTableWidgetItem(offset));
            tableWidget->setItem(tableWidget->rowCount() - 1, 4, new QTableWidgetItem(device));
            tableWidget->setItem(tableWidget->rowCount() - 1, 5, new QTableWidgetItem(inode));
            tableWidget->setItem(tableWidget->rowCount() - 1, 6, new QTableWidgetItem(fileName));
        }

        if (tableWidget->rowCount() == 0)
            tableWidget->setItem(0, 0, new QTableWidgetItem("Không tìm thấy shared memory (SYSV)."));
    });

    process->start("bash", QStringList() << "-c" << command);
}
