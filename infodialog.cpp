#include "infodialog.h"

InfoDialog::InfoDialog(const QString &title, const QString &content, QWidget *parent)
    : QDialog(parent) {
    setWindowTitle(title);

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Sử dụng QTextEdit để hiển thị thông tin
    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    textEdit->setText(content);

    layout->addWidget(textEdit);
    setLayout(layout);

    // Tùy chọn kích thước
    resize(600, 400);
}
