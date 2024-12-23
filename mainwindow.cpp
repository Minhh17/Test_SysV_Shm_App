#include "mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    // Tạo giao diện
    centralWidget = new QWidget(this);

    shmUpdateTimer = new QTimer(this);
    // Log Output
    logOutput = new QTextEdit(this);
    logOutput->setReadOnly(true);
    logOutput->setMinimumHeight(200); // Đặt chiều cao tối thiểu cho log

    // ComboBox4
    segmentSelector = new QComboBox(this);
    segmentSelector->setSizeAdjustPolicy(QComboBox::AdjustToContents); // Tự động điều chỉnh kích thước

    // GroupBox: Quản lý Shared Memory
    QGroupBox *groupMemory = new QGroupBox("Quản lý Shared Memory", this);
    QVBoxLayout *memoryLayout = new QVBoxLayout(groupMemory);
    QPushButton *btnCreate = new QPushButton("Tạo Shared Memory", this);
    QPushButton *btnAttach = new QPushButton("Attach vào Shm được chọn", this);
    QPushButton *btnDetach = new QPushButton("Detach Shm", this);
    memoryLayout->addWidget(btnCreate);
    memoryLayout->addWidget(btnAttach);
    memoryLayout->addWidget(btnDetach);

    // GroupBox: Dữ liệu
    QGroupBox *groupData = new QGroupBox("Dữ liệu", this);
    QVBoxLayout *dataLayout = new QVBoxLayout(groupData);
    QPushButton *btnWrite = new QPushButton("Ghi Dữ Liệu", this);
    QPushButton *btnRead = new QPushButton("Đọc Dữ Liệu", this);
    QPushButton *btnWriteRt = new QPushButton("Ghi Dữ Liệu Liên Tục", this);
    dataLayout->addWidget(btnWrite);
    dataLayout->addWidget(btnRead);
    dataLayout->addWidget(btnWriteRt);

    // GroupBox: Kiểm tra Shared Memory
    QGroupBox *groupManagement = new QGroupBox("Kiểm tra và quản lý", this);
    QVBoxLayout *managementLayout = new QVBoxLayout(groupManagement);
    QPushButton *btnCheck = new QPushButton("Kiểm Tra Shared Memory", this);
    QPushButton *btnDelete = new QPushButton("Xóa Shared Memory", this);
    QPushButton *btnRefresh = new QPushButton("Làm mới danh sách", this);
    managementLayout->addWidget(btnCheck);
    managementLayout->addWidget(btnDelete);
    managementLayout->addWidget(btnRefresh);

    // GroupBox: Address Manager
    QGroupBox *processManagement = new QGroupBox("Process", this);
    QVBoxLayout *processLayout = new QVBoxLayout(processManagement);
    QPushButton *btnReadMemMaps = new QPushButton("Đọc Memory Maps", this);
    QPushButton *btnShowShmPhyAddr = new QPushButton("Đọc Địa chỉ vật lý của Shm", this);
    QPushButton *btnPagemap = new QPushButton("xem bảng Pagemap", this);
    processLayout->addWidget(btnReadMemMaps);
    processLayout->addWidget(btnShowShmPhyAddr);
    processLayout->addWidget(btnPagemap);

    // Sử dụng QGridLayout
    QGridLayout *mainGrid = new QGridLayout(centralWidget);
    mainGrid->addWidget(logOutput, 0, 0, 1, 2);    // Log Output chiếm 2 cột
    mainGrid->addWidget(segmentSelector, 1, 0, 1, 2); // Segment Selector chiếm 2 cột
    mainGrid->addWidget(groupMemory, 2, 0);       // Nhóm quản lý Shared Memory
    mainGrid->addWidget(groupData, 2, 1);         // Nhóm dữ liệu
    mainGrid->addWidget(groupManagement, 3, 0);  // Nhóm kiểm tra và quản lý
    mainGrid->addWidget(processManagement, 3, 1); // Nhóm Process

    // Tinh chỉnh lưới
    mainGrid->setHorizontalSpacing(15); // Khoảng cách ngang giữa các widget
    mainGrid->setVerticalSpacing(15);   // Khoảng cách dọc giữa các widget
    mainGrid->setContentsMargins(10, 10, 10, 10); // Lề xung quanh lưới

    setCentralWidget(centralWidget);
    resize(900, 600); // Đặt kích thước cửa sổ
    setWindowTitle("PID: " + QString::number(QCoreApplication::applicationPid()));

    // Kết nối các nút bấm với chức năng
    connect(btnCreate, &QPushButton::clicked, this, &MainWindow::createSharedMemory);
    connect(btnAttach, &QPushButton::clicked, this, &MainWindow::attachSharedMemory);
    connect(btnDetach, &QPushButton::clicked, this, &MainWindow::detachSharedMemory);
    connect(btnWrite, &QPushButton::clicked, this, &MainWindow::writeSharedMemory);
    connect(btnRead, &QPushButton::clicked, this, &MainWindow::readSharedMemory);
    connect(btnCheck, &QPushButton::clicked, this, &MainWindow::checkSharedMemory);
    connect(btnDelete, &QPushButton::clicked, this, &MainWindow::deleteSharedMemory);
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::updateSegmentList);
    connect(btnReadMemMaps, &QPushButton::clicked, this, [this]() {
        MapsWindow *mapsWindow = new MapsWindow(this); // Tạo cửa sổ mới
        mapsWindow->loadMapsData(); // Nạp dữ liệu maps
        mapsWindow->show(); // Hiển thị cửa sổ
    });
    connect(btnShowShmPhyAddr, &QPushButton::clicked, this, &MainWindow::showShmPhyAddr);
    connect(btnWriteRt, &QPushButton::clicked, this, &MainWindow::updateSharedMemoryData);
    connect(btnPagemap, &QPushButton::clicked, this, &MainWindow::viewPageMap);
    // connect(btnReadMemMaps, &QPushButton::clicked, this, &MainWindow::readMemoryMaps);

    updateSegmentList();
}

MainWindow::~MainWindow() {}

void MainWindow::log(const QString &message) {
    logOutput->append(message);
}

void MainWindow::updateSegmentList() {
    QProcess process;
    process.start("ipcs -m");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();

    segmentSelector->clear();

    QStringList lines = output.split("\n");
    for (const QString &line : lines) {
        if (line.trimmed().isEmpty() || line.startsWith("------")) continue;

        QStringList columns = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
        if (columns.size() >= 2 && columns[0].at(0).isDigit()) {
            QString shmid = columns[1];
            QString key = columns[0];
            segmentSelector->addItem("Key: " + key + " (ID: " + shmid + ")", shmid);
        }
    }

    log("Danh sách shared memory đã được cập nhật.");
}

void MainWindow::createSharedMemory() {
    key_t key = ftok("/home/haiminh/Desktop/aptInstall.txt", rand() % 256);
    if (key == -1) {
        log("Lỗi: Không thể tạo key.");
        return;
    }

    int shmid = shmget(key, shmSize, IPC_CREAT | 0666);
    if (shmid == -1) {
        log("Lỗi: Không thể tạo shared memory.");
        return;
    }

    log("Đã tạo shared memory với ID: " + QString::number(shmid) + ", Key: " + QString::number(key));
    updateSegmentList();
}

void MainWindow::attachSharedMemory() {
    QString selectedText = segmentSelector->currentText();
    if (selectedText.isEmpty()) {
        log("Không có segment nào được chọn.");
        return;
    }

    int shmid = segmentSelector->currentData().toInt();
    // Kiểm tra nếu segment đã được gắn
    if (shmAttached.contains(shmid)) {
        log("Shared memory với ID: " + QString::number(shmid) + " đã được gắn trước đó.");
        return;
    }

    void *address = shmat(shmid, nullptr, 0);
    if (address == (void *)-1) {
        log("Lỗi: Không thể gắn shared memory với ID: " + QString::number(shmid));
        return;
    }

    shmAttached.insert(shmid, address);    
    log("Đã gắn shared memory với ID: " + QString::number(shmid));
    log("Địa chỉ vừa attach: " + QString::number((uintptr_t)address, 16));
    qDebug() << &address;
}

void MainWindow::detachSharedMemory() {
    QString selectedText = segmentSelector->currentText();
    int shmid = segmentSelector->currentData().toInt();
    if (!shmAttached.contains(shmid)) {
        log("Segment chưa được gắn, không thể detach.");
        return;
    }

    disconnect(shmUpdateTimer, &QTimer::timeout, this, &MainWindow::updateSharedMemoryData);

    void *address = shmAttached.value(shmid);
    if (shmdt(address) == -1) {
        perror("Lỗi shmdt");
        log("Lỗi: Không thể detach shared memory với ID: " + QString::number(shmid));
        return;
    }

    // Xóa khỏi danh sách gắn
    shmAttached.remove(shmid);
    log("Đã detach shared memory với ID: " + QString::number(shmid));
    log("Địa chỉ vừa detach: " + QString::number((uintptr_t)address, 16));    
}

void MainWindow::writeSharedMemory() {
    int shmid = segmentSelector->currentData().toInt();
    if (!shmAttached.contains(shmid)) {
        log("Shared memory chưa được gắn.");
        return;
    }

    QString data = QInputDialog::getText(this, "Nhập dữ liệu", "Dữ liệu để ghi:");
    if (data.isEmpty()) {
        log("Hủy ghi dữ liệu.");
        return;
    }

    void *address = shmAttached[shmid];
    strncpy((char *)address, data.toUtf8().constData(), shmSize);
    log("Đã ghi dữ liệu vào shared memory.");
}

void MainWindow::readSharedMemory() {
    int shmid = segmentSelector->currentData().toInt();
    if (!shmAttached.contains(shmid)) {
        log("Shared memory chưa được gắn.");
        return;
    }

    void *address = shmAttached[shmid];
    QString data = QString::fromUtf8((char *)address);
    log("Dữ liệu trong shared memory: " + data);
}

void MainWindow::checkSharedMemory() {
    int shmid = segmentSelector->currentData().toInt();
    struct shmid_ds shmInfo;

    if (shmctl(shmid, IPC_STAT, &shmInfo) == -1) {
        log("Lỗi: Không thể kiểm tra shared memory.");
        return;
    }

    log("Thông tin shared memory:");
    log("  Kích thước: " + QString::number(shmInfo.shm_segsz) + " bytes");
    log("  Số tiến trình gắn: " + QString::number(shmInfo.shm_nattch));
    log("  Permission: " + QString::number(shmInfo.shm_perm.uid));
    log("  PID của proc tạo shm: " + QString::number(shmInfo.shm_cpid));
    log("  PID của proc cuối truy cập: " + QString::number(shmInfo.shm_lpid));

    qInfo()<< shmInfo.shm_perm.uid << shmInfo.shm_cpid << shmInfo.shm_lpid;
}

void MainWindow::deleteSharedMemory() {
    int shmid = segmentSelector->currentData().toInt();
    if (shmctl(shmid, IPC_RMID, nullptr) == -1) {
        log("Lỗi: Không thể xóa shared memory.");
        return;
    }

    log("Đã xóa shared memory với ID: " + QString::number(shmid));
    updateSegmentList();
}

void MainWindow::readMemoryMaps() {
    QString pid = QString::number(QCoreApplication::applicationPid());
    qDebug() <<pid;
    QFile file("/proc/" + pid + "/maps");

    if (!file.exists()) {
        log("File /proc/" + pid + "/maps không tồn tại.");
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        log("Không thể mở /proc/" + pid + "/maps");
        return;
    }

    QTextStream in(&file);
    QString content;
    qDebug()<<in.readLine();
    while (!in.atEnd()) {
        content += in.readLine() + "\n";
    }
    file.close();

    if (content.isEmpty()) {
        log("Nội dung file maps rỗng.");
        return;
    }

    // Hiển thị thông tin trong InfoDialog
    InfoDialog *dialog = new InfoDialog("Memory Maps", content, this);
    dialog->exec();
}

uintptr_t MainWindow::getPhysicalAddress(uintptr_t virtualAddress) {
    QString pagemapFilePath = "/proc/self/pagemap";
    QFile pagemapFile(pagemapFilePath);

    if (!pagemapFile.open(QIODevice::ReadOnly)) {
        log("Không thể mở " + pagemapFilePath);
        return 0;
    }

    const int pageSize = sysconf(_SC_PAGESIZE);
    uintptr_t pageIndex = virtualAddress / pageSize;   // Tìm chỉ số trang
    uintptr_t pageOffset = virtualAddress % pageSize; // Tìm offset trong trang
    uintptr_t fileOffset = pageIndex * sizeof(uint64_t);

    if (!pagemapFile.seek(fileOffset)) {
        log("Không thể tìm đến offset trong pagemap.");
        pagemapFile.close();
        return 0;
    }

    uint64_t entry;
    if (pagemapFile.read(reinterpret_cast<char*>(&entry), sizeof(entry)) != sizeof(entry)) {
        log("Không thể đọc entry từ pagemap.");
        pagemapFile.close();
        return 0;
    }

    pagemapFile.close();

    bool present = entry & (1ULL << 63);  // Kiểm tra bit "present"
    if (!present) {
        log("Trang không được ánh xạ vào bộ nhớ vật lý.");
        return 0;
    }

    uintptr_t pfn = entry & ((1ULL << 55) - 1); // Trích xuất PFN (bits 0-54)
    uintptr_t physicalAddress = (pfn * pageSize) + pageOffset;

    return physicalAddress;
}

void MainWindow::showShmPhyAddr() {
    QString selectedText = segmentSelector->currentText();
    if (selectedText.isEmpty()) {
        log("Không có segment nào được chọn.");
        return;
    }

    int shmid = segmentSelector->currentData().toInt();
    if (!shmAttached.contains(shmid)) {
        log("Shared memory chưa được gắn.");
        return;
    }

    void *virtualAddress = shmAttached.value(shmid); // Lấy địa chỉ ảo
    // log("Địa chỉ ảo của shared memory: " + QString::number(reinterpret_cast<uintptr_t>(virtualAddress), 16));

    uintptr_t physicalAddress = getPhysicalAddress(reinterpret_cast<uintptr_t>(virtualAddress));
    if (physicalAddress == 0) {
        log("Không thể tìm thấy địa chỉ vật lý.");
    } else {
        // log("Địa chỉ vật lý của shared memory: 0x" + QString::number(physicalAddress, 16));
        log(QString("Shared Memory: VirtualAddress: 0x%1 -> PhysicalAddress: 0x%2")
                .arg(reinterpret_cast<uintptr_t>(virtualAddress), 0, 16)
                .arg(physicalAddress, 0, 16));
    }
}

void MainWindow::updateSharedMemoryData() {   // taij sao no ghi theo cap so nhan 2 mu x vay

    connect(shmUpdateTimer, &QTimer::timeout, this, &MainWindow::updateSharedMemoryData);
    shmUpdateTimer->start(1000); // Cập nhật mỗi 1000 ms
    // Lấy thời gian hiện tại
    QDateTime currentTime = QDateTime::currentDateTime();

    shmData = "Current Time: " + currentTime.toString("yyyy-MM-dd hh:mm:ss");

    // Ghi dữ liệu vào shared memory nếu đã attach
    int shmid = segmentSelector->currentData().toInt();
    if (shmAttached.contains(shmid)) {
        void *address = shmAttached[shmid];
        strncpy((char *)address, shmData.toUtf8().constData(), shmSize - 1);
        ((char *)address)[shmSize - 1] = '\0'; // Đảm bảo kết thúc chuỗi
        log("Đã ghi dữ liệu vào shared memory: " + shmData);
        shmData.clear();
    }
}

void MainWindow::viewPageMap() {
    if (shmAttached.isEmpty()) {
        log("Không có shared memory nào được gắn.");
        return;
    }

    // Lấy ID và địa chỉ ảo của shared memory được chọn
    int shmid = segmentSelector->currentData().toInt();
    if (!shmAttached.contains(shmid)) {
        log("Shared memory chưa được gắn.");
        return;
    }

    void *virtualAddress = shmAttached.value(shmid); // Địa chỉ ảo
    uintptr_t virtualAddrNumeric = reinterpret_cast<uintptr_t>(virtualAddress);

    log("Virtual Address của shared memory: " + QString::number(virtualAddrNumeric, 16));

    QString pagemapFilePath = "/proc/self/pagemap";
    QFile pagemapFile(pagemapFilePath);

    if (!pagemapFile.open(QIODevice::ReadOnly)) {
        log("Không thể mở " + pagemapFilePath);
        return;
    }

    const int pageSize = sysconf(_SC_PAGESIZE);
    uintptr_t pageIndex = virtualAddrNumeric / pageSize;   // Tìm chỉ số trang
    uintptr_t fileOffset = pageIndex * sizeof(uint64_t);

    if (!pagemapFile.seek(fileOffset)) {
        log("Không thể tìm đến offset trong pagemap.");
        pagemapFile.close();
        return;
    }

    uint64_t entry;
    if (pagemapFile.read(reinterpret_cast<char*>(&entry), sizeof(entry)) != sizeof(entry)) {
        log("Không thể đọc entry từ pagemap.");
        pagemapFile.close();
        return;
    }

    pagemapFile.close();

    // Phân tích các bit
    bool present = entry & (1ULL << 63);
    bool swapped = entry & (1ULL << 62);
    bool softDirty = entry & (1ULL << 55);
    bool exclusivelyMapped = entry & (1ULL << 56);
    bool isFilePageOrSharedAnon = entry & (1ULL << 61);

    QStringList pageMapInfo;
    pageMapInfo << QString("Entry Hex: 0x%1").arg(entry, 16, 16, QLatin1Char('0'));
    pageMapInfo << QString("Present (Bit 63): %1").arg(present ? "Yes" : "No");
    pageMapInfo << QString("Swapped (Bit 62): %1").arg(swapped ? "Yes" : "No");

    if (present) {
        uint64_t pfn = entry & ((1ULL << 55) - 1);
        uintptr_t physicalAddress = (pfn * pageSize) + (virtualAddrNumeric % pageSize);
        pageMapInfo << QString("PFN (Bits 0-54): 0x%1").arg(pfn, 0, 16);
        pageMapInfo << QString("Physical Address: 0x%1").arg(physicalAddress, 0, 16);
    } else if (swapped) {
        uint64_t swapType = entry & 0x1F;           // Bits 0-4
        uint64_t swapOffset = (entry >> 5) & 0x7FFFFFFFFFFFF; // Bits 5-54
        pageMapInfo << QString("Swap Type (Bits 0-4): %1").arg(swapType);
        pageMapInfo << QString("Swap Offset (Bits 5-54): %1").arg(swapOffset);
    }

    pageMapInfo << QString("Soft-dirty (Bit 55): %1").arg(softDirty ? "Yes" : "No");
    pageMapInfo << QString("Exclusively mapped (Bit 56): %1").arg(exclusivelyMapped ? "Yes" : "No");
    pageMapInfo << QString("Page type (Bit 61): %1").arg(isFilePageOrSharedAnon ? "File-page/Shared-anon" : "Khác");

    // Hiển thị thông tin trong hộp thoại
    QString info = pageMapInfo.join("\n");
    InfoDialog *dialog = new InfoDialog("Thông tin PageMap", info, this);
    dialog->exec();
}

