#include "color_wheel_widget.h"

#include <QPainter>
#include <QMouseEvent>
#include <QtMath>

ColorWheelWidget::ColorWheelWidget(QWidget* parent)
    : QWidget(parent) {
    setMinimumSize(200, 200);
}

void ColorWheelWidget::setColor(const QColor& c) {
    c.getHsvF(&hue_, &saturation_, &value_);
    update();
    emit colorChanged(color());
}

QColor ColorWheelWidget::color() const {
    QColor c;
    c.setHsvF(hue_, saturation_, value_);
    return c;
}

void ColorWheelWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int side = qMin(width(), height());
    const QPointF center(width() / 2.0, height() / 2.0);
    const qreal radius = side / 2.0 - 6.0;

    QImage img(side, side, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);

    for (int y = 0; y < side; ++y) {
        for (int x = 0; x < side; ++x) {
            const qreal dx = x - side / 2.0;
            const qreal dy = y - side / 2.0;
            const qreal r = std::sqrt(dx * dx + dy * dy);

            if (r > radius)
                continue;

            qreal angle = std::atan2(-dy, dx);
            qreal hue = std::fmod(angle / (2.0 * M_PI) + 1.0, 1.0);
            qreal sat = qMin(1.0, r / radius);

            QColor c;
            c.setHsvF(hue, sat, value_);
            img.setPixelColor(x, y, c);
        }
    }

    p.drawImage(
        QRectF(
            center.x() - side / 2.0,
            center.y() - side / 2.0,
            side,
            side
        ),
        img
    );

    // ---- Selector ----
    const qreal a = hue_ * 2.0 * M_PI;
    const qreal r = saturation_ * radius;

    QPointF sel(
        center.x() + r * std::cos(a),
        center.y() - r * std::sin(a)
    );

    p.setBrush(Qt::white);
    p.setPen(QPen(Qt::black, 2));
    p.drawEllipse(sel, 6, 6);
}

void ColorWheelWidget::mousePressEvent(QMouseEvent* e) {
    updateFromPosition(e->pos());
}

void ColorWheelWidget::mouseMoveEvent(QMouseEvent* e) {
    if (e->buttons() & Qt::LeftButton)
        updateFromPosition(e->pos());
}

void ColorWheelWidget::updateFromPosition(const QPoint& pos) {
    const QPointF center(width() / 2.0, height() / 2.0);
    QPointF d = pos - center;

    const qreal angle = std::atan2(-d.y(), d.x());
    const qreal dist  = std::sqrt(d.x() * d.x() + d.y() * d.y());

    hue_ = std::fmod((angle / (2.0 * M_PI)) + 1.0, 1.0);
    saturation_ = qMin(1.0, dist / (qMin(width(), height()) / 2.0));

    update();
    emit colorChanged(color());
}
