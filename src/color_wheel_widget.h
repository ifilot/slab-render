// SPDX-License-Identifier: LGPL-3.0-or-later
// SlabRender
// Author: Ivo Filot <ivo@ivofilot.nl>


#pragma once

#include <QWidget>
#include <QColor>

class ColorWheelWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief ColorWheelWidget.
     */
    explicit ColorWheelWidget(QWidget* parent = nullptr);

    /**
     * @brief setColor.
     */
    void setColor(const QColor& c);
    /**
     * @brief color.
     */
    QColor color() const;

protected:
    /**
     * @brief paintEvent.
     */
    void paintEvent(QPaintEvent*) override;
    /**
     * @brief mousePressEvent.
     */
    void mousePressEvent(QMouseEvent*) override;
    /**
     * @brief mouseMoveEvent.
     */
    void mouseMoveEvent(QMouseEvent*) override;

private:
    /**
     * @brief updateFromPosition.
     */
    void updateFromPosition(const QPoint& pos);

    qreal hue_        = 0.0;
    qreal saturation_ = 0.0;
    qreal value_      = 1.0;

signals:
    /**
     * @brief colorChanged.
     */
    void colorChanged(const QColor&);
};
