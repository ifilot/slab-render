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
#include <QListWidget>
#include <QColor>
#include <vector>

class RuleItemWidget;
class QListWidgetItem;

struct AtomColorRule {
    QString element;
    int from = 0;
    int to = 0;
    QColor color;
};

struct AtomRadiusRule {
    QString element;
    int from = 0;
    int to = 0;
    double radius = 1.0;
};

class RenderAtomsWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief RenderAtomsWidget.
     */
    explicit RenderAtomsWidget(QWidget* parent = nullptr);

    /**
     * @brief generate_json.
     */
    QString generate_json() const;

signals:
    /**
     * @brief rulesChanged.
     */
    void rulesChanged();

private slots:
    /**
     * @brief slot_add_color_rule.
     */
    void slot_add_color_rule();
    /**
     * @brief slot_add_radius_rule.
     */
    void slot_add_radius_rule();
    /**
     * @brief edit_rule.
     */
    void edit_rule(QListWidgetItem* item);

private:
    enum class RuleType {
        Color,
        Radius
    };

    /**
     * @brief build_ui.
     */
    void build_ui();
    void add_rule_item(RuleType type, int index,
                   const QString& text,
                   const QColor& color = QColor());
    /**
     * @brief delete_rule.
     */
    void delete_rule(QListWidgetItem* item);
    /**
     * @brief rebuild_rule_indices.
     */
    void rebuild_rule_indices();

    /**
     * @brief format_color_rule.
     */
    QString format_color_rule(const AtomColorRule& r) const;
    /**
     * @brief format_radius_rule.
     */
    QString format_radius_rule(const AtomRadiusRule& r) const;

    QListWidget* rule_list = nullptr;

    std::vector<AtomColorRule> color_rules;
    std::vector<AtomRadiusRule> radius_rules;
};
