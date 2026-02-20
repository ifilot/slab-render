/********************************************************************************
 * This file is part of SlabRender                                              *
 *                                                                              *
 * Author: Ivo Filot <i.a.w.filot@tue.nl>                                       *
 *                                                                              *
 * This program is free software; you can redistribute it and/or                *
 * modify it under the terms of the GNU Lesser General Public                   *
 * License as published by the Free Software Foundation; either                 *
 * version 3 of the License, or (at your option) any later version.             *
 *                                                                              *
 * This program is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU            *
 * Lesser General Public License for more details.                              *
 *                                                                              *
 * You should have received a copy of the GNU Lesser General Public License     *
 * along with this program; if not, write to the Free Software Foundation,      *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ********************************************************************************/

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
