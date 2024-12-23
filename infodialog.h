#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QString>

class InfoDialog : public QDialog {
    Q_OBJECT

public:
    explicit InfoDialog(const QString &title, const QString &content, QWidget *parent = nullptr);

private:
    QTextEdit *textEdit;
};

#endif // INFODIALOG_H
