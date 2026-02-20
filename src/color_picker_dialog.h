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

class ColorWheelWidget;
class QSlider;
class QLabel;

class ColorPickerDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief ColorPickerDialog.
     */
    explicit ColorPickerDialog(const QColor& initial, QWidget* parent = nullptr);
    /**
     * @brief color.
     */
    QColor color() const;

private:
    /**
     * @brief updateHexDisplay.
     */
    void updateHexDisplay(const QColor& c);

    ColorWheelWidget* wheel_;
    QSlider* valueSlider_;
    QLabel* hexLabel_;
};
