#pragma once
#include <QWidget>
#include <QColor>

class QLabel;
class QPushButton;
class QFrame;

class RuleItemWidget : public QWidget {
    Q_OBJECT

public:
    explicit RuleItemWidget(QWidget* parent = nullptr);

    void setText(const QString& text);
    void setColor(const QColor& color);
    void clearColor();

signals:
    void editRequested();
    void deleteRequested();

private:
    QLabel* label;
    QFrame* color_swatch;
    QPushButton* btn_edit;
    QPushButton* btn_delete;
};