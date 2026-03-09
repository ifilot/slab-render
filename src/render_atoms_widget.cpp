// SPDX-License-Identifier: LGPL-3.0-or-later
// SlabRender
// Author: Ivo Filot <ivo@ivofilot.nl>


#include "render_atoms_widget.h"
#include "rule_item_widget.h"
#include "periodic_table_dialog.h"
#include "rule_edit_dialog.h"
#include "color_picker_dialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

RenderAtomsWidget::RenderAtomsWidget(QWidget* parent)
    : QWidget(parent) {
    build_ui();
}

void RenderAtomsWidget::build_ui() {
    auto* layout = new QVBoxLayout(this);

    auto* btns = new QHBoxLayout();
    auto* btn_color = new QPushButton("Add atom color");
    auto* btn_radius = new QPushButton("Add atom radius");
    auto* btn_bond = new QPushButton("Add bond length");

    btns->addWidget(btn_color);
    btns->addWidget(btn_radius);
    btns->addWidget(btn_bond);
    layout->addLayout(btns);

    connect(btn_color, &QPushButton::clicked,
            this, &RenderAtomsWidget::slot_add_color_rule);
    connect(btn_radius, &QPushButton::clicked,
            this, &RenderAtomsWidget::slot_add_radius_rule);
    connect(btn_bond, &QPushButton::clicked,
            this, &RenderAtomsWidget::slot_add_bond_rule);

    rule_list = new QListWidget();
    rule_list->setSpacing(2);
    layout->addWidget(rule_list);
}

QString RenderAtomsWidget::format_color_rule(const AtomColorRule& r) const {
    return QString("<b>Color</b> <i>%1</i> [%2–%3] → %4")
        .arg(r.element)
        .arg(r.from)
        .arg(r.to)
        .arg(r.color.name());
}

QString RenderAtomsWidget::format_radius_rule(const AtomRadiusRule& r) const {
    return QString("<b>Radius</b> <i>%1</i> [%2–%3] → %4 Å")
        .arg(r.element)
        .arg(r.from)
        .arg(r.to)
        .arg(r.radius);
}

QString RenderAtomsWidget::format_bond_rule(const BondDistanceRule& r) const {
    return QString("<b>Bond</b> <i>%1-%2</i> → %3 Å")
        .arg(r.element_a)
        .arg(r.element_b)
        .arg(r.max_distance);
}

void RenderAtomsWidget::add_rule_item(RuleType type, int index, const QString& text, const QColor& color) {
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

    add_rule_item(RuleType::Color, index, format_color_rule(rule), rule.color);

    emit rulesChanged();
}

void RenderAtomsWidget::slot_add_radius_rule() {
    PeriodicTableDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    bool ok = false;
    double radius = QInputDialog::getDouble(this, tr("Atom radius"), tr("Radius (Å):"), 1.0, 0.01, 10.0, 2, &ok);

    if (!ok)
        return;

    AtomRadiusRule rule;
    rule.element = dlg.selectedElement();
    rule.radius  = radius;

    int index = static_cast<int>(radius_rules.size());
    radius_rules.push_back(rule);

    add_rule_item(RuleType::Radius, index, format_radius_rule(rule));

    emit rulesChanged();
}

void RenderAtomsWidget::slot_add_bond_rule() {
    PeriodicTableDialog dlg_a(this);
    if (dlg_a.exec() != QDialog::Accepted) {
        return;
    }

    PeriodicTableDialog dlg_b(this);
    if (dlg_b.exec() != QDialog::Accepted) {
        return;
    }

    bool ok = false;
    double max_distance = QInputDialog::getDouble(this, tr("Bond length"), tr("Max bond distance (Å):"), 2.0, 0.1, 10.0, 2, &ok);
    if (!ok) {
        return;
    }

    BondDistanceRule rule;
    rule.element_a = dlg_a.selectedElement();
    rule.element_b = dlg_b.selectedElement();
    rule.max_distance = max_distance;

    int index = static_cast<int>(bond_rules.size());
    bond_rules.push_back(rule);

    add_rule_item(RuleType::BondDistance, index, format_bond_rule(rule));
    emit rulesChanged();
}

void RenderAtomsWidget::delete_rule(QListWidgetItem* item) {
    RuleType type = static_cast<RuleType>(item->data(Qt::UserRole).toInt());
    int index = item->data(Qt::UserRole + 1).toInt();

    if (type == RuleType::Color && index < static_cast<int>(color_rules.size())) {
        color_rules.erase(color_rules.begin() + index);
    } else if (type == RuleType::Radius && index < static_cast<int>(radius_rules.size())) {
        radius_rules.erase(radius_rules.begin() + index);
    } else if (type == RuleType::BondDistance && index < static_cast<int>(bond_rules.size())) {
        bond_rules.erase(bond_rules.begin() + index);
    }

    delete rule_list->takeItem(rule_list->row(item));
    rebuild_rule_indices();
    emit rulesChanged();
}

void RenderAtomsWidget::edit_rule(QListWidgetItem* item) {
    RuleType type = static_cast<RuleType>(item->data(Qt::UserRole).toInt());
    int index = item->data(Qt::UserRole + 1).toInt();

    if (type == RuleType::Color) {
        if (index < 0 || index >= static_cast<int>(color_rules.size()))
            return;

        AtomColorRule& r = color_rules[index];

        RuleEditDialog dlg(RuleEditDialog::Mode::Color, r.element, r.from, r.to, r.color, 0.0, QString(), 0.0, this);

        if (dlg.exec() != QDialog::Accepted)
            return;

        r.element = dlg.element();
        r.from    = dlg.from();
        r.to      = dlg.to();
        r.color   = dlg.color();

        auto* widget = qobject_cast<RuleItemWidget*>(rule_list->itemWidget(item));
        widget->setText(format_color_rule(r));
        widget->setColor(r.color);

    } else if (type == RuleType::Radius) {
        if (index < 0 || index >= static_cast<int>(radius_rules.size()))
            return;

        AtomRadiusRule& r = radius_rules[index];

        RuleEditDialog dlg(RuleEditDialog::Mode::Radius, r.element, r.from, r.to, QColor(), r.radius, QString(), 0.0, this);

        if (dlg.exec() != QDialog::Accepted)
            return;

        r.element = dlg.element();
        r.from    = dlg.from();
        r.to      = dlg.to();
        r.radius  = dlg.radius();

        auto* widget = qobject_cast<RuleItemWidget*>(rule_list->itemWidget(item));
        widget->setText(format_radius_rule(r));
        widget->clearColor();
    } else if (type == RuleType::BondDistance) {
        if (index < 0 || index >= static_cast<int>(bond_rules.size()))
            return;

        BondDistanceRule& r = bond_rules[index];

        RuleEditDialog dlg(RuleEditDialog::Mode::BondDistance,
                           r.element_a,
                           0,
                           0,
                           QColor(),
                           0.0,
                           r.element_b,
                           r.max_distance,
                           this);

        if (dlg.exec() != QDialog::Accepted)
            return;

        r.element_a = dlg.element();
        r.element_b = dlg.element_b();
        r.max_distance = dlg.bond_distance();

        auto* widget = qobject_cast<RuleItemWidget*>(rule_list->itemWidget(item));
        widget->setText(format_bond_rule(r));
        widget->clearColor();
    }

    emit rulesChanged();
}

void RenderAtomsWidget::rebuild_rule_indices() {
    int color_index = 0;
    int radius_index = 0;
    int bond_index = 0;

    for (int i = 0; i < rule_list->count(); ++i) {
        auto* item = rule_list->item(i);
        RuleType type = static_cast<RuleType>(item->data(Qt::UserRole).toInt());

        if (type == RuleType::Color) {
            item->setData(Qt::UserRole + 1, color_index++);
        } else if (type == RuleType::Radius) {
            item->setData(Qt::UserRole + 1, radius_index++);
        } else {
            item->setData(Qt::UserRole + 1, bond_index++);
        }
    }
}

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

    if (!bond_rules.empty()) {
        QJsonArray arr;
        for (const auto& r : bond_rules) {
            arr.append(QString("%1/%2/%3")
                .arg(r.element_a)
                .arg(r.element_b)
                .arg(r.max_distance));
        }
        root["bond_distances"] = arr;
    }

    return QString(QJsonDocument(root).toJson(QJsonDocument::Indented));
}

void RenderAtomsWidget::load_from_json(const QJsonObject& root) {
    color_rules.clear();
    radius_rules.clear();
    bond_rules.clear();
    rule_list->clear();

    if(root.contains("atom_colors") && root["atom_colors"].isArray()) {
        const QJsonArray arr = root["atom_colors"].toArray();
        for(const auto& item : arr) {
            const QStringList pieces = item.toString().split('/');
            if(pieces.size() != 4) {
                continue;
            }

            AtomColorRule rule;
            rule.element = pieces[0];
            rule.from = pieces[1].toInt();
            rule.to = pieces[2].toInt();
            rule.color = QColor(pieces[3]);
            color_rules.push_back(rule);
            add_rule_item(RuleType::Color, static_cast<int>(color_rules.size()) - 1, format_color_rule(rule), rule.color);
        }
    }

    if(root.contains("atom_radii") && root["atom_radii"].isArray()) {
        const QJsonArray arr = root["atom_radii"].toArray();
        for(const auto& item : arr) {
            const QStringList pieces = item.toString().split('/');
            if(pieces.size() != 4) {
                continue;
            }

            AtomRadiusRule rule;
            rule.element = pieces[0];
            rule.from = pieces[1].toInt();
            rule.to = pieces[2].toInt();
            rule.radius = pieces[3].toDouble();
            radius_rules.push_back(rule);
            add_rule_item(RuleType::Radius, static_cast<int>(radius_rules.size()) - 1, format_radius_rule(rule));
        }
    }

    if(root.contains("bond_distances") && root["bond_distances"].isArray()) {
        const QJsonArray arr = root["bond_distances"].toArray();
        for(const auto& item : arr) {
            const QStringList pieces = item.toString().split('/');
            if(pieces.size() != 3) {
                continue;
            }

            BondDistanceRule rule;
            rule.element_a = pieces[0];
            rule.element_b = pieces[1];
            rule.max_distance = pieces[2].toDouble();
            bond_rules.push_back(rule);
            add_rule_item(RuleType::BondDistance, static_cast<int>(bond_rules.size()) - 1, format_bond_rule(rule));
        }
    }

    emit rulesChanged();
}
