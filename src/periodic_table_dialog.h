#pragma once

#include <QDialog>
#include <QString>

class QGridLayout;

class PeriodicTableDialog : public QDialog {
    Q_OBJECT

public:
    explicit PeriodicTableDialog(QWidget* parent = nullptr);

    QString selectedElement() const;

private:
    void build_ui();
    void add_element(int row, int col, unsigned int elnr);
    void add_placeholder(int row, int col, const QString& text, const QString& tooltip);

    QGridLayout* grid_ = nullptr;
    QString selected_;
};
