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

#include "render_atoms_widget.h"
#include "rule_item_widget.h"
#include "periodic_table_dialog.h"
#include "rule_edit_dialog.h"
#include "color_picker_dialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QColorDialog>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

RenderAtomsWidget::RenderAtomsWidget(QWidget* parent)
    : QWidget(parent) {
    build_ui();
}

/**
 * @brief build_ui.
 */
void RenderAtomsWidget::build_ui() {
    auto* layout = new QVBoxLayout(this);

    // Buttons
    auto* btns = new QHBoxLayout();
    auto* btn_color = new QPushButton("Add atom color");
    auto* btn_radius = new QPushButton("Add atom radius");

    btns->addWidget(btn_color);
    btns->addWidget(btn_radius);
    layout->addLayout(btns);

    connect(btn_color, &QPushButton::clicked,
            this, &RenderAtomsWidget::slot_add_color_rule);
    connect(btn_radius, &QPushButton::clicked,
            this, &RenderAtomsWidget::slot_add_radius_rule);

    // Rule list
    rule_list = new QListWidget();
    rule_list->setSpacing(2);
    layout->addWidget(rule_list);
}

/**
 * @brief format_color_rule.
 */
QString RenderAtomsWidget::format_color_rule(const AtomColorRule& r) const {
    return QString(
        "<b>Color</b> <i>%1</i> [%2–%3] → %4"
    )
        .arg(r.element)
        .arg(r.from)
        .arg(r.to)
        .arg(r.color.name());
}

/**
 * @brief format_radius_rule.
 */
QString RenderAtomsWidget::format_radius_rule(const AtomRadiusRule& r) const {
    return QString(
        "<b>Radius</b> <i>%1</i> [%2–%3] → %4 Å"
    )
        .arg(r.element)
        .arg(r.from)
        .arg(r.to)
        .arg(r.radius);
}

void RenderAtomsWidget::add_rule_item(
    RuleType type,
    int index,
    const QString& text,
    const QColor& color
) {
    auto* item = new QListWidgetItem(rule_list);
    auto* widget = new RuleItemWidget();

    item->setSizeHint(widget->sizeHint());
    item->setData(Qt::UserRole, static_cast<int>(type));
    item->setData(Qt::UserRole + 1, index);

    widget->setText(text);

    if (type == RuleType::Color) {
        widget->setColor(color);
    } else {
        widget->clearColor();
    }

    rule_list->addItem(item);
    rule_list->setItemWidget(item, widget);

    connect(widget, &RuleItemWidget::deleteRequested, this, [this, item]() {
        delete_rule(item);
    });

    connect(widget, &RuleItemWidget::editRequested, this, [this, item]() {
        edit_rule(item);
    });
}

/**
 * @brief slot_add_color_rule.
 */
void RenderAtomsWidget::slot_add_color_rule() {
    PeriodicTableDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    ColorPickerDialog colorDlg(Qt::white, this);
    if (colorDlg.exec() != QDialog::Accepted)
        return;

    QColor color = colorDlg.color();
    if (!color.isValid())
        return;

    AtomColorRule rule;
    rule.element = dlg.selectedElement();
    rule.color   = color;

    int index = static_cast<int>(color_rules.size());
    color_rules.push_back(rule);

    add_rule_item(
        RuleType::Color,
        index,
        format_color_rule(rule),
        rule.color
    );

    emit rulesChanged();
}

/**
 * @brief slot_add_radius_rule.
 */
void RenderAtomsWidget::slot_add_radius_rule() {
    PeriodicTableDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    bool ok = false;
    double radius = QInputDialog::getDouble(
        this,
        tr("Atom radius"),
        tr("Radius (Å):"),
        1.0,
        0.01,
        10.0,
        2,
        &ok
    );

    if (!ok)
        return;

    AtomRadiusRule rule;
    rule.element = dlg.selectedElement();
    rule.radius  = radius;

    int index = static_cast<int>(radius_rules.size());
    radius_rules.push_back(rule);

    add_rule_item(
        RuleType::Radius,
        index,
        format_radius_rule(rule)
    );

    emit rulesChanged();
}

/**
 * @brief delete_rule.
 */
void RenderAtomsWidget::delete_rule(QListWidgetItem* item) {
    RuleType type =
        static_cast<RuleType>(item->data(Qt::UserRole).toInt());
    int index = item->data(Qt::UserRole + 1).toInt();

    if (type == RuleType::Color && index < static_cast<int>(color_rules.size())) {
        color_rules.erase(color_rules.begin() + index);
    } else if (type == RuleType::Radius && index < static_cast<int>(radius_rules.size())) {
        radius_rules.erase(radius_rules.begin() + index);
    }

    delete rule_list->takeItem(rule_list->row(item));
    rebuild_rule_indices();
    emit rulesChanged();
}

/**
 * @brief edit_rule.
 */
void RenderAtomsWidget::edit_rule(QListWidgetItem* item) {
    RuleType type =
        static_cast<RuleType>(item->data(Qt::UserRole).toInt());
    int index = item->data(Qt::UserRole + 1).toInt();

    if (type == RuleType::Color) {
        if (index < 0 || index >= static_cast<int>(color_rules.size()))
            return;

        AtomColorRule& r = color_rules[index];

        RuleEditDialog dlg(
            RuleEditDialog::Mode::Color,
            r.element,
            r.from,
            r.to,
            r.color,
            0.0,
            this
        );

        if (dlg.exec() != QDialog::Accepted)
            return;

        // Apply edits
        r.element = dlg.element();
        r.from    = dlg.from();
        r.to      = dlg.to();
        r.color   = dlg.color();

        // Update UI text
        auto* widget =
            qobject_cast<RuleItemWidget*>(rule_list->itemWidget(item));
        widget->setText(format_color_rule(r));
        widget->setColor(r.color);

    } else if (type == RuleType::Radius) {
        if (index < 0 || index >= static_cast<int>(radius_rules.size()))
            return;

        AtomRadiusRule& r = radius_rules[index];

        RuleEditDialog dlg(
            RuleEditDialog::Mode::Radius,
            r.element,
            r.from,
            r.to,
            QColor(),
            r.radius,
            this
        );

        if (dlg.exec() != QDialog::Accepted)
            return;

        // Apply edits
        r.element = dlg.element();
        r.from    = dlg.from();
        r.to      = dlg.to();
        r.radius  = dlg.radius();

        // Update UI text
        auto* widget =
            qobject_cast<RuleItemWidget*>(rule_list->itemWidget(item));
        widget->setText(format_radius_rule(r));
        widget->clearColor();
    }

    emit rulesChanged();
}

/**
 * @brief rebuild_rule_indices.
 */
void RenderAtomsWidget::rebuild_rule_indices() {
    int color_index = 0;
    int radius_index = 0;

    for (int i = 0; i < rule_list->count(); ++i) {
        auto* item = rule_list->item(i);
        RuleType type =
            static_cast<RuleType>(item->data(Qt::UserRole).toInt());

        if (type == RuleType::Color) {
            item->setData(Qt::UserRole + 1, color_index++);
        } else {
            item->setData(Qt::UserRole + 1, radius_index++);
        }
    }
}

/**
 * @brief generate_json.
 */
QString RenderAtomsWidget::generate_json() const {
    QJsonObject root;

    if (!color_rules.empty()) {
        QJsonArray arr;
        for (const auto& r : color_rules) {
            arr.append(QString("%1/%2/%3/%4")
                .arg(r.element)
                .arg(r.from)
                .arg(r.to)
                .arg(r.color.name()));
        }
        root["atom_colors"] = arr;
    }

    if (!radius_rules.empty()) {
        QJsonArray arr;
        for (const auto& r : radius_rules) {
            arr.append(QString("%1/%2/%3/%4")
                .arg(r.element)
                .arg(r.from)
                .arg(r.to)
                .arg(r.radius));
        }
        root["atom_radii"] = arr;
    }

    return QString(QJsonDocument(root).toJson(QJsonDocument::Indented));
}
