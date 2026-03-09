// SPDX-License-Identifier: LGPL-3.0-or-later
// SlabRender
// Author: Ivo Filot <ivo@ivofilot.nl>


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
    QPushButton* button_render_single_file;
    QPushButton* button_save_blend_file;

    QPlainTextEdit* text_job_info;
    QLabel* label_image;
    QLabel* label_selected_atom;

    ThreadRenderImage* process_job_queue = nullptr;
    AnaglyphWidget* anaglyph_widget = nullptr;
    int current_job_id = -1;

    QString get_expected_image_path(const QString& filepath) const;

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
    void signal_render_single_job_requested(int job_id);
    void signal_save_blend_single_job_requested(int job_id);

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

    /**
     * @brief slot_render_single_file.
     */
    void slot_render_single_file();

    /**
     * @brief slot_save_blend_single_file.
     */
    void slot_save_blend_single_file();
};
