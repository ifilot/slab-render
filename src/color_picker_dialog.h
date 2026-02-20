// SPDX-License-Identifier: LGPL-3.0-or-later
// SlabRender
// Author: Ivo Filot <ivo@ivofilot.nl>


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
