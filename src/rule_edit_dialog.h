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

    QString element() const;
    int from() const;
    int to() const;
    QColor color() const;
    double radius() const;

private slots:
    void slot_select_element();
    void slot_select_color();

private:
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
