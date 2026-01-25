#include "periodic_table_dialog.h"
#include <QGridLayout>
#include <QPushButton>

PeriodicTableDialog::PeriodicTableDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle(tr("Select atom"));
    setModal(true);
    build_ui();
}

void PeriodicTableDialog::build_ui() {
    auto* layout = new QGridLayout(this);

    // Minimal set (expand later or auto-generate)
    QStringList elements = {
        "H","He",
        "Li","Be","B","C","N","O","F","Ne"
    };

    int row = 0, col = 0;
    for (const QString& el : elements) {
        auto* btn = new QPushButton(el);
        btn->setFixedSize(36, 36);

        connect(btn, &QPushButton::clicked,
                this, &PeriodicTableDialog::slot_element_clicked);

        layout->addWidget(btn, row, col++);
        if (col > 4) { col = 0; row++; }
    }
}

void PeriodicTableDialog::slot_element_clicked() {
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    selected_element = btn->text();
    accept();  // close dialog with QDialog::Accepted
}

QString PeriodicTableDialog::selectedElement() const {
    return selected_element;
}
