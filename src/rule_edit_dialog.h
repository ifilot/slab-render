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
        Radius
    };

    explicit RuleEditDialog(
        Mode mode,
        const QString& element,
        int from,
        int to,
        const QColor& color,
        double radius,
        QWidget* parent = nullptr
    );

    /**
     * @brief element.
     */
    QString element() const;
    /**
     * @brief from.
     */
    int from() const;
    /**
     * @brief to.
     */
    int to() const;
    /**
     * @brief color.
     */
    QColor color() const;
    /**
     * @brief radius.
     */
    double radius() const;

private slots:
    /**
     * @brief slot_select_element.
     */
    void slot_select_element();
    /**
     * @brief slot_select_color.
     */
    void slot_select_color();

private:
    /**
     * @brief update_color_button.
     */
    void update_color_button();

    Mode mode_;

    QString element_;
    QColor color_;

    QPushButton* btn_element = nullptr;
    QPushButton* btn_color   = nullptr;

    QSpinBox* spin_from = nullptr;
    QSpinBox* spin_to   = nullptr;
    QDoubleSpinBox* spin_radius = nullptr;
};
