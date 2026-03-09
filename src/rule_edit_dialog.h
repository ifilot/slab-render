// SPDX-License-Identifier: LGPL-3.0-or-later
// SlabRender
// Author: Ivo Filot <ivo@ivofilot.nl>


#pragma once

#include <QDialog>
#include <QColor>

class QLabel;
class QPushButton;
class QSpinBox;
class QDoubleSpinBox;

class RuleEditDialog : public QDialog {
    Q_OBJECT

public:
    enum class Mode {
        Color,
        Radius,
        BondDistance
    };

    explicit RuleEditDialog(
        Mode mode,
        const QString& element,
        int from,
        int to,
        const QColor& color,
        double radius,
        const QString& element_b = QString(),
        double bond_distance = 2.0,
        QWidget* parent = nullptr
    );

    QString element() const;
    QString element_b() const;
    int from() const;
    int to() const;
    QColor color() const;
    double radius() const;
    double bond_distance() const;

private slots:
    void slot_select_element();
    void slot_select_color();

private:
    void update_color_button();

    Mode mode_;

    QString element_;
    QString element_b_;
    QColor color_;

    QPushButton* btn_element = nullptr;
    QPushButton* btn_element_b = nullptr;
    QPushButton* btn_color   = nullptr;

    QSpinBox* spin_from = nullptr;
    QSpinBox* spin_to   = nullptr;
    QDoubleSpinBox* spin_radius = nullptr;
    QDoubleSpinBox* spin_bond_distance = nullptr;
};
