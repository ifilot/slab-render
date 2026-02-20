// SPDX-License-Identifier: LGPL-3.0-or-later
// SlabRender
// Author: Ivo Filot <ivo@ivofilot.nl>


#pragma once

#include <QDialog>
#include <QString>

class QGridLayout;

class PeriodicTableDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief PeriodicTableDialog.
     */
    explicit PeriodicTableDialog(QWidget* parent = nullptr);

    /**
     * @brief selectedElement.
     */
    QString selectedElement() const;

private:
    /**
     * @brief build_ui.
     */
    void build_ui();
    /**
     * @brief add_element.
     */
    void add_element(int row, int col, unsigned int elnr);
    /**
     * @brief add_placeholder.
     */
    void add_placeholder(int row, int col, const QString& text, const QString& tooltip);

    QGridLayout* grid_ = nullptr;
    QString selected_;
};
