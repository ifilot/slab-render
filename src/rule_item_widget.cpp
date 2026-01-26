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

void RuleItemWidget::setText(const QString& text) {
    label->setText(text);
}

void RuleItemWidget::setColor(const QColor& color) {
    color_swatch->setStyleSheet(
        QString("background-color: %1;").arg(color.name())
    );
    color_swatch->setVisible(true);
}

void RuleItemWidget::clearColor() {
    color_swatch->setVisible(false);
}
