#ifndef PageMapEntry_H
#define PageMapEntry_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

class PageMapEntry : public QDialog {
    Q_OBJECT

public:
    PageMapEntry(uint64_t entry, uintptr_t virtualAddress, QWidget *parent = nullptr);
};

#endif // PageMapEntry_H
