// SPDX-License-Identifier: LGPL-3.0-or-later
// SlabRender
// Author: Ivo Filot <ivo@ivofilot.nl>

#pragma once

#include <QThread>
#include <QObject>
#include <QStringList>
#include <QDir>
#include <QTemporaryDir>
#include <QProcess>
#include <QTextStream>
#include <QMap>

#include <fstream>
#include <cstdint>
#include <chrono>

#include "structure_loader.h"

class ThreadRenderImage : public QThread
{
    Q_OBJECT
private:
    enum class RenderMode {
        RenderImage,
        SaveBlend
    };

    QStringList files;

    QString executable;

    StructureLoader sl;

    QVector<QStringList> output;

    QMap<QString, QVariant> parameters;

    QVector<double> process_times;

    int single_job_id = -1;

    uint32_t principal_nr_atoms = 0;

    RenderMode render_mode = RenderMode::RenderImage;

public:
   /**
    * @brief ThreadRenderImage.
    */
    ThreadRenderImage();

    /**
     * @brief set_files.
     */
    inline void set_files(const QStringList& _files) {
        this->files = _files;
        this->output.resize(this->files.count());
        this->process_times.resize(this->files.count());
    }

    /**
     * @brief set_single_job_id.
     */
    inline void set_single_job_id(int _job_id) {
        this->single_job_id = _job_id;
    }

    /**
     * @brief set_executable.
     */
    inline void set_executable(const QString& _executable) {
        this->executable = _executable;
    }

    /**
     * @brief get_output.
     */
    inline const QStringList& get_output(int id) const {
        return this->output[id];
    }

    /**
     * @brief get_process_time.
     */
    inline double get_process_time(int id) const {
        return this->process_times[id];
    }

    /**
     * @brief get_file.
     */
    inline const QString& get_file(int id) const {
        return this->files[id];
    }

    /**
     * @brief set_parameters.
     */
    inline void set_parameters(const QMap<QString, QVariant>& _parameters) {
        this->parameters = _parameters;
    }

    /**
     * @brief set_render_mode.
     */
    inline void set_render_mode(bool save_blend) {
        this->render_mode = save_blend ? RenderMode::SaveBlend : RenderMode::RenderImage;
    }

    /**
     * @brief run.
     */
    void run();

private:
    /**
     * @brief build_process.
     */
    QProcess* build_process(const QString& working_directory);

    /**
     * @brief copy_template_files.
     */
    QString copy_template_files();

    /**
     * @brief create_atompack.
     */
    void create_atompack(const QString& structure_path, const QString& output_path);

    /**
     * @brief build_manifest_file.
     */
    void build_manifest_file(const QString& path);

signals:
    /**
     * @brief signal_job_done.
     */
    void signal_job_done(int jobid);

    /**
     * @brief signal_job_start.
     */
    void signal_job_start(int jobid);

    /**
     * @brief signal_queue_done.
     */
    void signal_queue_done();

    /**
     * @brief signal_queue_cancelled.
     */
    void signal_queue_cancelled();
};
