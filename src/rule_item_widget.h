// SPDX-License-Identifier: LGPL-3.0-or-later
// SlabRender
// Author: Ivo Filot <ivo@ivofilot.nl>


#pragma once
#include <QWidget>
#include <QColor>

class QLabel;
class QPushButton;
class QFrame;

class RuleItemWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief RuleItemWidget.
     */
    explicit RuleItemWidget(QWidget* parent = nullptr);

    /**
     * @brief setText.
     */
    void setText(const QString& text);
    /**
     * @brief setColor.
     */
    void setColor(const QColor& color);
    /**
     * @brief clearColor.
     */
    void clearColor();

signals:
    /**
     * @brief editRequested.
     */
    void editRequested();
    /**
     * @brief deleteRequested.
     */
    void deleteRequested();

private:
    QLabel* label;
    QFrame* color_swatch;
    QPushButton* btn_edit;
    QPushButton* btn_delete;
};
