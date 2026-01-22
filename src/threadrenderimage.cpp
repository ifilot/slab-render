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
#include "threadrenderimage.h"

ThreadRenderImage::ThreadRenderImage() {

}

void ThreadRenderImage::run() {
    qDebug() << "Running Blender for " << this->files.count() << " structures.";
    for(int i=0; i<this->files.count(); i++) {

        if(this->single_job_id >= 0 && this->single_job_id != i) {
            continue;
        }

        if(isInterruptionRequested()) {
            qDebug() << "Interruption received: cancelling queue.";
            emit(signal_queue_cancelled());
            break;
        }

        const QString& file = this->files[i];
        qDebug() << "Parsing: " << file;
        this->create_atompack(file);
        QProcess* process = this->build_process(file);

        // emit job start
        emit(signal_job_start(i));
        auto start = std::chrono::steady_clock::now();

        process->start();
        qDebug() << "Blender process launched";
        if(process->waitForStarted(1000)) {
            qDebug() << "Blender process started";
            if(process->waitForFinished(60 * 60 * 1000)) { // timeout at one hour
                qDebug() << "Blender process finished";

                // collect output of job
                QStringList result;
                auto lines = process->readAll().split('\n');
                for(const QByteArray& line : lines) {
                    result << line;
                }

                // read any errors
                auto error_lines = process->readAllStandardError().split('\n');
                for(const QByteArray& line : error_lines) {
                    result << line;
                }

                // store output of job
                this->output[i] = result;

                // copy image back
                QFile imagefile(process->workingDirectory() + "/image.png");
                if(imagefile.open(QIODevice::ReadOnly)) {
                    QString storepath = QFileInfo(file).absoluteDir().path() + "/image.png";

                    // remove existing file if it exists
                    if(QFile::exists(storepath)) {
                        QFile::remove(storepath);
                    }
                    imagefile.copy(storepath);
                }

                // emit job done
                auto end = std::chrono::steady_clock::now();
                std::chrono::duration<double> elapsed_seconds = end-start;
                this->process_times[i] = elapsed_seconds.count();
                emit(signal_job_done(i));
            } else {
                qCritical() << "Process did not finish";
            }
        } else {
            qCritical() << "Process did not launch";
            qCritical() << process->errorString();
        }

        // clean up folder
        QDir dir(process->workingDirectory());
        dir.removeRecursively();
    }

    // emit all jobs are processed
    emit(signal_queue_done());
}

QProcess* ThreadRenderImage::build_process(const QString& contcarfile) {
    QString cwd = this->copy_template_files(contcarfile);
    QStringList arguments = {"-b", "axes_template.blend", "-P", "render_image.py", "--", "manifest.json", "atompack.bin", cwd + "/image.png"};
    QProcess* blender_process = new QProcess();
    blender_process->setProgram(this->executable);
    blender_process->setArguments(arguments);
    blender_process->setProcessChannelMode(QProcess::SeparateChannels);
    blender_process->setWorkingDirectory(cwd);

    return blender_process;
}

QString ThreadRenderImage::copy_template_files(const QString& contcarfile) {
    QTemporaryDir dir;
    dir.setAutoRemove(false); // do not immediately remove
    if(dir.isValid()) {
        // write Blender axes template file
        QFile blenderfile(":/assets/blender/axes_template.blend");
        if(!blenderfile.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("Could not open blender file from assets.");
        }
        blenderfile.copy(dir.path() + "/axes_template.blend");

        // copy atompack.bin
        QString atompackpath = QFileInfo(contcarfile).absoluteDir().path() + "/atompack.bin";
        QFile atompackfile(atompackpath);
        if(atompackfile.open(QIODevice::ReadOnly)) {
            atompackfile.copy(dir.path() + "/atompack.bin");
        }

        // write Python file containing Blender instructions
        QFile pythonfile(":/assets/blender/render_image.py");
        if(!pythonfile.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("Could not open Python file from assets.");
        }
        pythonfile.copy(dir.path() + "/render_image.py");

        // copy atoms.json file
        QFile atomsjson(":/assets/configuration/atoms.json");
        if(!atomsjson.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("Could not open blender file from assets.");
        }
        atomsjson.copy(dir.path() + "/atoms.json");

        // write JSON manifest file
        this->build_manifest_file(dir.path() + "/manifest.json");
    } else {
        throw std::runtime_error("Invalid path");
    }

    return QDir::cleanPath(dir.path());
}

void ThreadRenderImage::create_atompack(const QString& path) {
    qDebug() << "Converting CONTCAR to atompack.bin for " << path;
    try {
        auto structure = sl.load_file(path).back();
        structure->update();

        // ToDo: Convert to Qt based file handling routines

        // writing results to file
        auto storepath = QFileInfo(path).absoluteDir().path() + "/atompack.bin";
        qDebug() << "Storing " << storepath;
        std::ofstream out(storepath.toStdString(), std::ios::out | std::ios::binary);

        // write unit cell
        MatrixUnitcell mat = structure->get_unitcell();
        for(unsigned int i=0; i<3; i++) {
            for(unsigned int j=0; j<3; j++) {
                double val = mat(i,j);
                out.write((char*)&val, sizeof(double));
            }
        }

        // write atoms
        uint32_t nr_atoms = structure->get_nr_atoms();
        out.write((char*)&nr_atoms, sizeof(uint32_t));
        for(const auto& atom : structure->get_atoms()) {
            const uint8_t atnr = atom.atnr;
            out.write((char*)&atnr, sizeof(uint8_t));
            out.write((char*)&atom.x, sizeof(double));
            out.write((char*)&atom.y, sizeof(double));
            out.write((char*)&atom.z, sizeof(double));
        }

        // write bonds
        const uint32_t nr_bonds = structure->get_bonds().size();
        out.write((char*)&nr_bonds, sizeof(uint32_t));
        for(const auto& bond : structure->get_bonds()) {
            const uint8_t atnr1 = bond.atom1.atnr;
            out.write((char*)&atnr1, sizeof(uint8_t));                  // atom 1
            const uint8_t atnr2 = bond.atom2.atnr;
            out.write((char*)&atnr2, sizeof(uint8_t));                  // atom 2
            out.write((char*)&bond.atom_id_1, sizeof(uint16_t));
            out.write((char*)&bond.atom_id_2, sizeof(uint16_t));
            out.write((char*)&bond.axis[0], sizeof(double) * 3);        // axis
            out.write((char*)&bond.angle, sizeof(double));              // angle
            out.write((char*)&bond.length, sizeof(double));             // length
        }

        // write expansion atoms
        uint32_t nr_expansion_atoms = structure->get_expansion_atoms().size();
        out.write((char*)&nr_expansion_atoms, sizeof(uint32_t));
        for(const auto& atom : structure->get_expansion_atoms()) {
            const uint8_t atnr = atom.atnr;
            out.write((char*)&atnr, sizeof(uint8_t));
            out.write((char*)&atom.x, sizeof(double));
            out.write((char*)&atom.y, sizeof(double));
            out.write((char*)&atom.z, sizeof(double));
        }

        // write bonds
        const uint32_t nr_expansion_bonds = structure->get_expansion_bonds().size();
        out.write((char*)&nr_expansion_bonds, sizeof(uint32_t));
        for(const auto& bond : structure->get_expansion_bonds()) {
            const uint8_t atnr1 = bond.atom1.atnr;
            out.write((char*)&atnr1, sizeof(uint8_t));                  // atom 1
            const uint8_t atnr2 = bond.atom2.atnr;
            out.write((char*)&atnr2, sizeof(uint8_t));                  // atom 2
            out.write((char*)&bond.atom_id_1, sizeof(uint16_t));
            out.write((char*)&bond.atom_id_2, sizeof(uint16_t));
            out.write((char*)&bond.axis[0], sizeof(double) * 3);        // axis
            out.write((char*)&bond.angle, sizeof(double));              // angle
            out.write((char*)&bond.length, sizeof(double));             // length
        }

        out.close();
    }  catch (const std::exception& e) {
        qCritical() << "Error encountered: " << e.what();
    }
}

void ThreadRenderImage::build_manifest_file(const QString& path) {
    QFile outfile(path);
    if(outfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&outfile);

        stream << "{" << "\n";

        QStringList string_parameters = {"bondmat", "atmat", "camera_direction"};
        QStringList bool_parameters = {"expansion", "hide_axes", "show_unitcell"};
        QStringList int_parameters = {"resolution_x", "resolution_y", "tile_x", "tile_y", "samples", "nsubdiv"};

        try {
            // whether to have regular ortho scale or custom
            if(this->parameters["ortho_scale"] == "auto") {
                stream << "\"" << "ortho_scale" << "\": " << "\"" << this->parameters["ortho_scale"].toString() << "\"" << ",\n";
            } else {
                stream << "\"" << "ortho_scale" << "\": " << "\"" << this->parameters["ortho_custom_scale"].toString() << "\"" << ",\n";
            }

            for(const QString& str : string_parameters) {
                stream << "\"" << str << "\": " << "\"" << this->parameters[str].toString() << "\"" << ",\n";
            }

            for(const QString& str : bool_parameters) {
                stream << "\"" << str << "\": " << tr(this->parameters[str].toBool() ? "true" : "false") << ",\n";
            }

            for(const QString& str : int_parameters) {
                stream << "\"" << str << "\": " << this->parameters[str].toInt() << ",\n";
            }
        }  catch (const std::exception& e) {
            qCritical() << tr("Error encountered in parsing parameters: ") + tr(e.what());
        }

        stream << this->parameters["custom_json"].toString() << "\n";

        stream << "\"generator\": \"SlabRender\"\n";

        stream << "}" << "\n";

        outfile.close();
    }
}
