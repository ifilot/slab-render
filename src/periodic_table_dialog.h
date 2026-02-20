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
