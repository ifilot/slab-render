#pragma once
#include <QDialog>
#include <QColor>

class QSlider;
class QLabel;
class QLineEdit;

class ColorPickerDialog : public QDialog {
    Q_OBJECT

public:
    explicit ColorPickerDialog(const QColor& initial, QWidget* parent = nullptr);

    QColor color() const;

private slots:
    void slot_slider_changed();
    void slot_hex_changed();

private:
    void update_ui();

    QColor color_;

    QLabel* preview;
    QSlider* slider_r;
    QSlider* slider_g;
    QSlider* slider_b;
    QLineEdit* edit_hex;
};
