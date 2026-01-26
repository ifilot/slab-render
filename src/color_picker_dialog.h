#pragma once

#include <QDialog>
#include <QColor>

class ColorWheelWidget;
class QSlider;
class QLabel;

class ColorPickerDialog : public QDialog {
    Q_OBJECT

public:
    explicit ColorPickerDialog(const QColor& initial, QWidget* parent = nullptr);
    QColor color() const;

private:
    void updateHexDisplay(const QColor& c);

    ColorWheelWidget* wheel_;
    QSlider* valueSlider_;
    QLabel* hexLabel_;
};
