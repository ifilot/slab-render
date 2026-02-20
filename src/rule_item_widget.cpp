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

#include "rule_item_widget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>

RuleItemWidget::RuleItemWidget(QWidget* parent)
    : QWidget(parent) {

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(6);

    label = new QLabel();
    label->setTextFormat(Qt::RichText);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    color_swatch = new QFrame();
    color_swatch->setFixedSize(16, 16);
    color_swatch->setFrameShape(QFrame::Box);
    color_swatch->setVisible(false);

    btn_edit = new QPushButton("Edit");
    btn_edit->setFixedWidth(44);

    btn_delete = new QPushButton("✖");
    btn_delete->setFixedWidth(28);

    // 👇 ORDER MATTERS HERE
    layout->addWidget(label);         // text first
    layout->addWidget(color_swatch);  // color swatch after hex
    layout->addWidget(btn_edit);
    layout->addWidget(btn_delete);

    connect(btn_edit, &QPushButton::clicked,
            this, &RuleItemWidget::editRequested);
    connect(btn_delete, &QPushButton::clicked,
            this, &RuleItemWidget::deleteRequested);
}

/**
 * @brief setText.
 */
void RuleItemWidget::setText(const QString& text) {
    label->setText(text);
}

/**
 * @brief setColor.
 */
void RuleItemWidget::setColor(const QColor& color) {
    color_swatch->setStyleSheet(
        QString("background-color: %1;").arg(color.name())
    );
    color_swatch->setVisible(true);
}

/**
 * @brief clearColor.
 */
void RuleItemWidget::clearColor() {
    color_swatch->setVisible(false);
}
