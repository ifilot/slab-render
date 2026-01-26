#pragma once

#include <QWidget>
#include <QColor>

class ColorWheelWidget : public QWidget {
    Q_OBJECT

public:
    explicit ColorWheelWidget(QWidget* parent = nullptr);

    void setColor(const QColor& c);
    QColor color() const;

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;

private:
    void updateFromPosition(const QPoint& pos);

    qreal hue_        = 0.0;
    qreal saturation_ = 0.0;
    qreal value_      = 1.0;

signals:
    void colorChanged(const QColor&);
};
