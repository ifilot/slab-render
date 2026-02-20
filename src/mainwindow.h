/********************************************************************************
 * This file is part of SlabRender                                                *
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

#include <QApplication>
#include <QMainWindow>
#include <QStringList>
#include <QDir>
#include <QDirIterator>
#include <QProcess>
#include <QTemporaryDir>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGridLayout>
#include <QFileDialog>
#include <QLabel>
#include <QScrollArea>
#include <QListWidget>
#include <QIcon>
#include <QProgressBar>
#include <QPlainTextEdit>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QMap>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include <QTextCursor>
#include <QSettings>
#include <QJsonObject>

#include "jobinfowidget.h"
#include "threadrenderimage.h"
#include "logwindow.h"
#include "config.h"
#include "vendor/simpleson/json.h"
#include "atom_settings.h"
#include "render_atoms_widget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QStringList executables;
    QListWidget* listview_items;
    QPushButton* button_probe_gpu;
    QComboBox* combobox_file_types;
    QPushButton* button_parse_files;
    QPushButton* button_run_single_job;
    QPushButton* button_cancel;
    QPushButton* button_select_folder;
    QProgressBar* progress_bar;
    QLabel* label_gpus;

    std::unique_ptr<ThreadRenderImage> process_job_queue;

    // blender settings
    QComboBox* combobox_blender_executable;
    QComboBox* combobox_ortho_scale;
    QLabel* label_custom_ortho_scale;
    QDoubleSpinBox* spinbox_custom_ortho_scale;
    QComboBox* combobox_camera_direction;
    QDoubleSpinBox* spinbox_custom_euler_x;
    QDoubleSpinBox* spinbox_custom_euler_y;
    QDoubleSpinBox* spinbox_custom_euler_z;
    bool flag_block_custom_euler_sync = false;
    QCheckBox* checkbox_unitcell;
    QCheckBox* checkbox_expansion;
    QCheckBox* checkbox_axes;
    QSpinBox* spinbox_resolution_x;
    QSpinBox* spinbox_resolution_y;
    QSpinBox* spinbox_tile_x;
    QSpinBox* spinbox_tile_y;
    QSpinBox* spinbox_samples;
    QSpinBox* spinbox_nsubdiv;
    QComboBox* combobox_atom_material;
    QComboBox* combobox_bond_material;
    QLabel* label_valid_json;

    // storage for log messages
    std::shared_ptr<QStringList> log_messages;

    // window for log messages
    std::unique_ptr<LogWindow> log_window;

    RenderAtomsWidget* render_atoms_widget = nullptr;
    QGroupBox* advanced_json_group = nullptr;
    JobInfoWidget* widget_job_info = nullptr;

    QVector<unsigned int> job_status;

    enum {
        JOB_QUEUED,
        JOB_RUNNING,
        JOB_COMPLETED,
        JOB_CANCELLED
    };

    const QStringList GEOMETRY_FILETYPES {
        "VASP Geometry (POSCAR*,CONTCAR*)",
        "ADF .log files (logfile)",
        "Gaussian .log files (*.log, *.LOG)",
        "MKMCXX3 .mks files (*.mks)",
    };

public:
    MainWindow(const std::shared_ptr<QStringList> _log_messages,
               QWidget *parent = nullptr);
    /**
     * @brief MainWindow.
     */
    ~MainWindow();

private:
    /**
     * @brief build_dropdown_menu.
     */
    void build_dropdown_menu();

    /**
     * @brief build_blender_settings_panel.
     */
    void build_blender_settings_panel(QVBoxLayout* layout);

    /**
     * @brief find_blender_executable.
     */
    QStringList find_blender_executable();

    /**
     * @brief Find files with a specific file name
     */
    QStringList find_files(const QString& path, const QStringList& filenames);

    /**
     * @brief fetch_tooltip_text.
     */
    QString fetch_tooltip_text(const QString& filename);

private slots:
    /**
     * @brief slot_select_folder.
     */
    void slot_select_folder();

    /**
     * @brief slot_parse_files.
     */
    void slot_parse_files();

    /**
     * @brief slot_parse_single_job.
     */
    void slot_parse_single_job();

    /**
     * @brief slot_job_start.
     */
    void slot_job_start(int jobid);

    /**
     * @brief slot_job_done.
     */
    void slot_job_done(int jobid);

    /**
     * @brief slot_queue_done.
     */
    void slot_queue_done();

    /**
     * @brief slot_probe_gpu.
     */
    void slot_probe_gpu();

    /**
     * @brief slot_change_ortho_scale.
     */
    void slot_change_ortho_scale(int item_id);

    /**
     * @brief slot_update_custom_zoom_level.
     */
    void slot_update_custom_zoom_level();
    /**
     * @brief slot_update_custom_euler.
     */
    void slot_update_custom_euler();
    /**
     * @brief slot_set_custom_euler.
     */
    void slot_set_custom_euler();

    /**
     * @brief slot_sync_atom_render_rules.
     */
    void slot_sync_atom_render_rules();

    /**
     * @brief slot_cancel_queue.
     */
    void slot_cancel_queue();

    /**
     * @brief slot_queue_cancelled.
     */
    void slot_queue_cancelled();

    /**
     * @brief slot_exit.
     */
    void slot_exit();

    /**
     * @brief slot_debug_log.
     */
    void slot_debug_log();

    /**
     * @brief slot_about.
     */
    void slot_about();

private:
    /**
     * @brief build_custom_json.
     */
    QJsonObject build_custom_json() const;
};
