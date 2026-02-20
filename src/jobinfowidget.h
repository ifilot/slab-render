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

#include <QWidget>
#include <QPlainTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QFileDialog>

#include "threadrenderimage.h"
#include "visualization/anaglyph_widget.h"

class JobInfoWidget : public QTabWidget
{
    Q_OBJECT

private:
    QLabel* label_job_path;
    QPushButton* button_open_path;
    QPushButton* button_save_image;

    QPlainTextEdit* text_job_info;
    QLabel* label_image;
    QLabel* label_selected_atom;

    ThreadRenderImage* process_job_queue = nullptr;
    AnaglyphWidget* anaglyph_widget = nullptr;

public:
    /**
     * @brief JobInfoWidget.
     */
    explicit JobInfoWidget(QWidget *parent = nullptr);

    /**
     * @brief set_process_job_queue_ptr.
     */
    inline void set_process_job_queue_ptr(ThreadRenderImage* _process_job_queue) {
        this->process_job_queue = _process_job_queue;
    }

    /**
     * @brief get_anaglyph_widget.
     */
    inline AnaglyphWidget* get_anaglyph_widget() {
        return this->anaglyph_widget;
    }

    /**
     * @brief Rebuild structures based on AtomSettings data
     */
    void rebuild_structures();

signals:

public slots:
    /**
     * @brief slot_update_job_info.
     */
    void slot_update_job_info(int job_id);

private slots:
    /**
     * @brief slot_update_atom_label.
     */
    void slot_update_atom_label(int atom_id);

    /**
     * @brief slot_show_path_in_explorer_window.
     */
    void slot_show_path_in_explorer_window();

    /**
     * @brief slot_save_image.
     */
    void slot_save_image();
};
