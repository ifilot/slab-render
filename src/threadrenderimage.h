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

#include <QThread>
#include <QObject>
#include <QStringList>
#include <QDir>
#include <QTemporaryDir>
#include <QProcess>
#include <QTextStream>
#include <QMap>

#include <fstream>
#include <chrono>

#include "structure_loader.h"

class ThreadRenderImage : public QThread
{
    Q_OBJECT
private:
    QStringList files;

    QString executable;

    StructureLoader sl;

    QVector<QStringList> output;

    QMap<QString, QVariant> parameters;

    QVector<double> process_times;

    int single_job_id = -1;

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
     * @brief run.
     */
    void run();

private:
    /**
     * @brief build_process.
     */
    QProcess* build_process(const QString& executable);

    /**
     * @brief copy_template_files.
     */
    QString copy_template_files(const QString& contcarfile);

    /**
     * @brief create_atompack.
     */
    void create_atompack(const QString& contcarpath);

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
