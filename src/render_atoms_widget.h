// SPDX-License-Identifier: LGPL-3.0-or-later
// SlabRender
// Author: Ivo Filot <ivo@ivofilot.nl>


#pragma once

#include <QWidget>
#include <QListWidget>
#include <QColor>
#include <QJsonObject>
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

struct BondDistanceRule {
    QString element_a;
    QString element_b;
    double max_distance = 2.0;
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

    /**
     * @brief load_from_json.
     */
    void load_from_json(const QJsonObject& root);

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
     * @brief slot_add_bond_rule.
     */
    void slot_add_bond_rule();
    /**
     * @brief edit_rule.
     */
    void edit_rule(QListWidgetItem* item);

private:
    enum class RuleType {
        Color,
        Radius,
        BondDistance
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
    /**
     * @brief format_bond_rule.
     */
    QString format_bond_rule(const BondDistanceRule& r) const;

    QListWidget* rule_list = nullptr;

    std::vector<AtomColorRule> color_rules;
    std::vector<AtomRadiusRule> radius_rules;
    std::vector<BondDistanceRule> bond_rules;
};
