#ifndef MAPSWINDOW_H
#define MAPSWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QProcess>

class MapsWindow : public QWidget {
    Q_OBJECT

public:
    explicit MapsWindow(QWidget *parent = nullptr);
    void loadMapsData(); // Hàm để đọc dữ liệu từ /proc/[PID]/maps

private:
    QTableWidget *tableWidget; // Bảng hiển thị thông tin maps
};

#endif // MAPSWINDOW_H
