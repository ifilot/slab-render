// SPDX-License-Identifier: LGPL-3.0-or-later
// SlabRender
// Author: Ivo Filot <ivo@ivofilot.nl>


#include "rule_edit_dialog.h"
#include "periodic_table_dialog.h"
#include "color_picker_dialog.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>

RuleEditDialog::RuleEditDialog(
    Mode mode,
    const QString& element,
    int from,
    int to,
    const QColor& color,
    double radius,
    const QString& element_b,
    double bond_distance,
    QWidget* parent
)
    : QDialog(parent),
      mode_(mode),
      element_(element),
      element_b_(element_b),
      color_(color) {

    setWindowTitle(tr("Edit atom rule"));
    setModal(true);
    setMinimumWidth(320);

    auto* mainLayout = new QVBoxLayout(this);
    auto* grid = new QGridLayout();
    mainLayout->addLayout(grid);

    int row = 0;

    grid->addWidget(new QLabel(mode_ == Mode::BondDistance ? tr("Element A") : tr("Element")), row, 0);

    btn_element = new QPushButton(element_);
    btn_element->setToolTip(tr("Select atom type"));
    grid->addWidget(btn_element, row, 1);

    connect(btn_element, &QPushButton::clicked,
            this, &RuleEditDialog::slot_select_element);
    row++;

    if(mode_ == Mode::BondDistance) {
        grid->addWidget(new QLabel(tr("Element B")), row, 0);
        btn_element_b = new QPushButton(element_b_);
        btn_element_b->setToolTip(tr("Select second atom type"));
        grid->addWidget(btn_element_b, row, 1);

        connect(btn_element_b, &QPushButton::clicked,
                this, &RuleEditDialog::slot_select_element);
        row++;

        grid->addWidget(new QLabel(tr("Max bond distance (Å)")), row, 0);
        spin_bond_distance = new QDoubleSpinBox();
        spin_bond_distance->setRange(0.1, 10.0);
        spin_bond_distance->setDecimals(2);
        spin_bond_distance->setSingleStep(0.05);
        spin_bond_distance->setValue(bond_distance);
        grid->addWidget(spin_bond_distance, row, 1);
        row++;
    } else {
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
    }

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
        if(sender() == btn_element_b) {
            element_b_ = dlg.selectedElement();
            btn_element_b->setText(element_b_);
        } else {
            element_ = dlg.selectedElement();
            btn_element->setText(element_);
        }
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

QString RuleEditDialog::element() const {
    return element_;
}

QString RuleEditDialog::element_b() const {
    return element_b_;
}

int RuleEditDialog::from() const {
    return spin_from ? spin_from->value() : 0;
}

int RuleEditDialog::to() const {
    return spin_to ? spin_to->value() : 0;
}

QColor RuleEditDialog::color() const {
    return color_;
}

double RuleEditDialog::radius() const {
    return spin_radius ? spin_radius->value() : 0.0;
}

double RuleEditDialog::bond_distance() const {
    return spin_bond_distance ? spin_bond_distance->value() : 0.0;
}
