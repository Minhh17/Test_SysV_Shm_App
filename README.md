TestShmIPC là một ứng dụng quản lý và kiểm tra Shared Memory (Bộ nhớ chia sẻ) trong môi trường Linux. Ứng dụng được xây dựng bằng Qt C++ và được đóng gói sử dụng CMake. Đây là một công cụ hỗ trợ cho các nhà phát triển hoặc quản trị viên hệ thống để làm việc với Shared Memory IPC (Inter-Process Communication).
Các tính năng chính

    Quản lý Shared Memory:
        Tạo và xóa các phân đoạn Shared Memory.
        Gắn (attach) và tách (detach) các phân đoạn Shared Memory.
        Kiểm tra thông tin của Shared Memory như kích thước, số tiến trình đang gắn, PID của tiến trình tạo hoặc sử dụng gần nhất.

    Ghi/Đọc dữ liệu:
        Ghi dữ liệu vào Shared Memory.
        Đọc dữ liệu từ Shared Memory.

    Quản lý tiến trình và bộ nhớ:
        Đọc các file /proc/[PID]/maps để kiểm tra các phân đoạn bộ nhớ.
        Hiển thị địa chỉ vật lý tương ứng với địa chỉ ảo của Shared Memory thông qua file /proc/self/pagemap.

    Liên tục cập nhật Shared Memory:
        Ghi dữ liệu vào Shared Memory theo thời gian thực, ví dụ: đồng bộ hóa thời gian hiện tại.

    Giao diện người dùng dễ sử dụng:
        Giao diện được xây dựng bằng Qt Widgets, có các nút bấm, hộp chọn (ComboBox), và khung hiển thị nhật ký (log).
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
