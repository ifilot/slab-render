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
    explicit RenderAtomsWidget(QWidget* parent = nullptr);

    QString generate_json() const;

signals:
    void rulesChanged();

private slots:
    void slot_add_color_rule();
    void slot_add_radius_rule();
    void edit_rule(QListWidgetItem* item);

private:
    enum class RuleType {
        Color,
        Radius
    };

    void build_ui();
    void add_rule_item(RuleType type, int index,
                   const QString& text,
                   const QColor& color = QColor());
    void delete_rule(QListWidgetItem* item);
    void rebuild_rule_indices();

    QString format_color_rule(const AtomColorRule& r) const;
    QString format_radius_rule(const AtomRadiusRule& r) const;

    QListWidget* rule_list = nullptr;

    std::vector<AtomColorRule> color_rules;
    std::vector<AtomRadiusRule> radius_rules;
};
