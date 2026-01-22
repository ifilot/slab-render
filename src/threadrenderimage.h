/********************************************************************************
 * This file is part of Saucepan                                                *
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
#ifndef THREADRENDERIMAGE_H
#define THREADRENDERIMAGE_H

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
    ThreadRenderImage();

    inline void set_files(const QStringList& _files) {
        this->files = _files;
        this->output.resize(this->files.count());
        this->process_times.resize(this->files.count());
    }

    inline void set_single_job_id(int _job_id) {
        this->single_job_id = _job_id;
    }

    inline void set_executable(const QString& _executable) {
        this->executable = _executable;
    }

    inline const QStringList& get_output(int id) const {
        return this->output[id];
    }

    inline double get_process_time(int id) const {
        return this->process_times[id];
    }

    inline const QString& get_file(int id) const {
        return this->files[id];
    }

    inline void set_parameters(const QMap<QString, QVariant>& _parameters) {
        this->parameters = _parameters;
    }

    void run();

private:
    QProcess* build_process(const QString& executable);

    QString copy_template_files(const QString& contcarfile);

    void create_atompack(const QString& contcarpath);

    void build_manifest_file(const QString& path);

signals:
    void signal_job_done(int jobid);

    void signal_job_start(int jobid);

    void signal_queue_done();

    void signal_queue_cancelled();
};

#endif // THREADRENDERIMAGE_H
