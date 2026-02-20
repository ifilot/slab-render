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
