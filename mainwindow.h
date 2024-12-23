#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QInputDialog>
#include <QMap>
#include <QProcess>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <cstring>
#include <QCoreApplication>
#include <QPageLayout>
#include <QFile>
#include "infodialog.h"
#include "mapswindow.h"
#include "pagemapentry.h"
#include <QTimer>
#include <QDateTime>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    void createSharedMemory();      // Tạo Shared Memory
    void attachSharedMemory();      // Gắn Shared Memory từ danh sách
    void detachSharedMemory();
    void writeSharedMemory();       // Ghi dữ liệu vào Shared Memory
    void readSharedMemory();        // Đọc dữ liệu từ Shared Memory
    void checkSharedMemory();       // Kiểm tra Shared Memory
    void deleteSharedMemory();      // Xóa Shared Memory
    void updateSegmentList();       // Cập nhật danh sách segment hiện có
    void readMemoryMaps();
    void showShmPhyAddr();
    void updateSharedMemoryData();
    void viewPageMap();

private:
    QWidget *centralWidget;
    QTextEdit *logOutput;           // Hiển thị log
    QComboBox *segmentSelector;     // Danh sách các Shared Memory segment
    QPushButton *btnCreate;
    QPushButton *btnAttach;
    QPushButton *btnDetach;
    QPushButton *btnWrite;
    QPushButton *btnRead;
    QPushButton *writeRealtime;
    QPushButton *btnCheck;
    QPushButton *btnDelete;
    QPushButton *btnRefresh;
    QPushButton *readMemMaps;
    QPushButton *btnShowShmPhyAddr;
    QPushButton *viewPagemap;

    QTimer *shmUpdateTimer; // Timer để cập nhật shared memory
    QString shmData; // Dữ liệu sẽ được ghi vào shared memory

    QMap<int, void*> shmAttached;   // Quản lý các segment đã gắn
    const int shmSize = 1024;       // Kích thước mỗi segment
    void log(const QString &message); // Thêm log vào giao diện

    // uintptr_t getPhysicalAddress(uintptr_t *virtualAddress);
    uintptr_t getPhysicalAddress(uintptr_t virtualAddress);

};

#endif // MAINWINDOW_H
