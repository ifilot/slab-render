#pragma once
#include <QDialog>

class QPushButton;

class PeriodicTableDialog : public QDialog {
    Q_OBJECT

public:
    explicit PeriodicTableDialog(QWidget* parent = nullptr);

    QString selectedElement() const;

private slots:
    void slot_element_clicked();

private:
    void build_ui();

    QString selected_element;
};
