#include "rule_edit_dialog.h"
#include "periodic_table_dialog.h"
#include "color_picker_dialog.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QColorDialog>
#include <QHBoxLayout>

RuleEditDialog::RuleEditDialog(
    Mode mode,
    const QString& element,
    int from,
    int to,
    const QColor& color,
    double radius,
    QWidget* parent
)
    : QDialog(parent),
      mode_(mode),
      element_(element),
      color_(color) {

    setWindowTitle(tr("Edit atom rule"));
    setModal(true);
    setMinimumWidth(320);

    auto* mainLayout = new QVBoxLayout(this);
    auto* grid = new QGridLayout();
    mainLayout->addLayout(grid);

    int row = 0;

    // --- Element selector ---
    grid->addWidget(new QLabel(tr("Element")), row, 0);

    btn_element = new QPushButton(element_);
    btn_element->setToolTip(tr("Select atom type"));
    grid->addWidget(btn_element, row, 1);

    connect(btn_element, &QPushButton::clicked,
            this, &RuleEditDialog::slot_select_element);
    row++;

    // --- Atom range ---
    grid->addWidget(new QLabel(tr("From atom")), row, 0);
    spin_from = new QSpinBox();
    spin_from->setRange(0, 9999);
    spin_from->setValue(from);
    spin_from->setToolTip(tr("0 means all atoms"));
    grid->addWidget(spin_from, row, 1);
    row++;

    grid->addWidget(new QLabel(tr("To atom")), row, 0);
    spin_to = new QSpinBox();
    spin_to->setRange(0, 9999);
    spin_to->setValue(to);
    spin_to->setToolTip(tr("0 means all atoms"));
    grid->addWidget(spin_to, row, 1);
    row++;

    // --- Color OR Radius ---
    if (mode_ == Mode::Color) {
        grid->addWidget(new QLabel(tr("Color")), row, 0);

        btn_color = new QPushButton();
        btn_color->setFixedHeight(28);
        update_color_button();

        grid->addWidget(btn_color, row, 1);

        connect(btn_color, &QPushButton::clicked,
                this, &RuleEditDialog::slot_select_color);

    } else {
        grid->addWidget(new QLabel(tr("Radius (Å)")), row, 0);

        spin_radius = new QDoubleSpinBox();
        spin_radius->setRange(0.01, 10.0);
        spin_radius->setDecimals(2);
        spin_radius->setSingleStep(0.05);
        spin_radius->setValue(radius);

        grid->addWidget(spin_radius, row, 1);
    }

    // --- Buttons ---
    auto* buttonLayout = new QHBoxLayout();
    mainLayout->addLayout(buttonLayout);
    buttonLayout->addStretch();

    auto* btn_cancel = new QPushButton(tr("Cancel"));
    auto* btn_ok     = new QPushButton(tr("OK"));

    btn_ok->setDefault(true);

    buttonLayout->addWidget(btn_cancel);
    buttonLayout->addWidget(btn_ok);

    connect(btn_ok, &QPushButton::clicked,
            this, &QDialog::accept);
    connect(btn_cancel, &QPushButton::clicked,
            this, &QDialog::reject);
}

void RuleEditDialog::slot_select_element() {
    PeriodicTableDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        element_ = dlg.selectedElement();
        btn_element->setText(element_);
    }
}

void RuleEditDialog::slot_select_color() {
    ColorPickerDialog dlg(color_, this);
    if (dlg.exec() == QDialog::Accepted) {
        color_ = dlg.color();
        update_color_button();
    }
}

void RuleEditDialog::update_color_button() {
    btn_color->setText(color_.name());
    btn_color->setStyleSheet(
        QString(
            "QPushButton {"
            " background-color: %1;"
            " color: %2;"
            " border: 1px solid #444;"
            " }"
        ).arg(color_.name(),
              color_.lightness() < 128 ? "#ffffff" : "#000000")
    );
}

// --- Getters ---

QString RuleEditDialog::element() const {
    return element_;
}

int RuleEditDialog::from() const {
    return spin_from->value();
}

int RuleEditDialog::to() const {
    return spin_to->value();
}

QColor RuleEditDialog::color() const {
    return color_;
}

double RuleEditDialog::radius() const {
    return spin_radius ? spin_radius->value() : 0.0;
}
