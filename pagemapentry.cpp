#include "pagemapentry.h"

PageMapEntry::PageMapEntry(uint64_t entry, uintptr_t virtualAddress, QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("Page Map Entry Details");

    QGridLayout *layout = new QGridLayout(this);

    // Entry Value (Hex)
    QLabel *entryLabel = new QLabel("Entry Value (Hex):", this);
    QLineEdit *entryValue = new QLineEdit(this);
    entryValue->setText(QString::number(entry, 16));
    entryValue->setReadOnly(true);
    layout->addWidget(entryLabel, 0, 0);
    layout->addWidget(entryValue, 0, 1);

    // Virtual Address (Hex)
    QLabel *virtualAddressLabel = new QLabel("Virtual Address (Hex):", this);
    QLineEdit *virtualAddressValue = new QLineEdit(this);
    virtualAddressValue->setText(QString::number(virtualAddress, 16));
    virtualAddressValue->setReadOnly(true);
    layout->addWidget(virtualAddressLabel, 1, 0);
    layout->addWidget(virtualAddressValue, 1, 1);

    // Page Frame Number (PFN) (Hex)
    QLabel *pfnLabel = new QLabel("Page Frame Number (PFN) (Hex):", this);
    QLineEdit *pfnValue = new QLineEdit(this);
    uintptr_t pfn = entry & ((1ULL << 55) - 1);
    pfnValue->setText(QString::number(pfn, 16));
    pfnValue->setReadOnly(true);
    layout->addWidget(pfnLabel, 2, 0);
    layout->addWidget(pfnValue, 2, 1);

    // Page Present
    QLabel *presentLabel = new QLabel("Page Present:", this);
    QLineEdit *presentValue = new QLineEdit(this);
    presentValue->setText((entry & (1ULL << 63)) ? "Yes" : "No");
    presentValue->setReadOnly(true);
    layout->addWidget(presentLabel, 3, 0);
    layout->addWidget(presentValue, 3, 1);

    // Page Swapped
    QLabel *swappedLabel = new QLabel("Page Swapped:", this);
    QLineEdit *swappedValue = new QLineEdit(this);
    swappedValue->setText((entry & (1ULL << 62)) ? "Yes" : "No");
    swappedValue->setReadOnly(true);
    layout->addWidget(swappedLabel, 4, 0);
    layout->addWidget(swappedValue, 4, 1);

    setLayout(layout);
}
