// SPDX-License-Identifier: LGPL-3.0-or-later
// SlabRender
// Author: Ivo Filot <ivo@ivofilot.nl>


#include "jobinfowidget.h"

/**
 * @brief JobInfoWidget.
 */
JobInfoWidget::JobInfoWidget(QWidget *parent) : QTabWidget(parent) {
    // create info tab
    QWidget* widget_info_container = new QWidget();
    this->insertTab(0, widget_info_container, "Job info");

    QVBoxLayout* layout = new QVBoxLayout();
    widget_info_container->setLayout(layout);

    this->label_job_path = new QLabel();
    layout->addWidget(this->label_job_path);

    QWidget* button_container = new QWidget();
    QHBoxLayout* layout_button_container = new QHBoxLayout();
    button_container->setLayout(layout_button_container);
    layout->addWidget(button_container);
    this->button_open_path = new QPushButton("Open path");
    layout_button_container->addWidget(this->button_open_path);
    this->button_open_path->setEnabled(false);
    connect(this->button_open_path, SIGNAL(released()), this, SLOT(slot_show_path_in_explorer_window()));
    this->button_save_image = new QPushButton("Save image as");
    layout_button_container->addWidget(this->button_save_image);
    this->button_save_image->setEnabled(false);
    connect(this->button_save_image, SIGNAL(released()), this, SLOT(slot_save_image()));

    this->label_image = new QLabel();
    this->label_image->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->label_image->setAlignment (Qt::AlignCenter);
    layout->addWidget(this->label_image);

    layout->addWidget(new QLabel("Rendering log"));
    this->text_job_info = new QPlainTextEdit();
    this->text_job_info->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    layout->addWidget(this->text_job_info);
    this->text_job_info->setReadOnly(true);
    this->text_job_info->setOverwriteMode(false);

    // add anaglyph widget
    this->anaglyph_widget = new AnaglyphWidget();
    this->anaglyph_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    QWidget* anaglyph_container = new QWidget();
    anaglyph_container->setLayout(new QVBoxLayout());
    this->insertTab(1, anaglyph_container, "Structure");
    anaglyph_container->layout()->addWidget(this->anaglyph_widget);

    this->label_selected_atom = new QLabel("Atom selection");
    anaglyph_container->layout()->addWidget(this->label_selected_atom);

    connect(this->anaglyph_widget, SIGNAL(signal_atom_selected(int)), this, SLOT(slot_update_atom_label(int)));
}

/**
 * @brief Rebuild structures based on AtomSettings data
 */
void JobInfoWidget::rebuild_structures() {
    qDebug() << "Rebuilding structures based on new JSON data";
    this->anaglyph_widget->get_structure()->update();
    this->anaglyph_widget->update();
}

/**
 * @brief slot_update_job_info.
 */
void JobInfoWidget::slot_update_job_info(int job_id) {
    qDebug() << "Updating job info for job id: " << job_id;
    if(this->process_job_queue != nullptr) {
        this->text_job_info->clear();
        this->text_job_info->appendPlainText(this->process_job_queue->get_output(job_id).join('\n'));
        QString contcarpath = this->process_job_queue->get_file(job_id);
        this->label_job_path->setText(contcarpath);
        this->button_open_path->setEnabled(true);

        QString imagepath = QFileInfo(contcarpath).absoluteDir().path() + "/image.png";
        QFile imagefile(imagepath);
        if(imagefile.exists()) {
            QPixmap pixmap(imagepath);
            this->label_image->setPixmap(pixmap.scaled(this->label_image->width(),this->label_image->height(),Qt::KeepAspectRatio));
            this->label_image->setStyleSheet("border: 1px solid black;");
            this->button_save_image->setEnabled(true);
        } else {
            this->label_image->clear();
            this->label_image->setStyleSheet("");
            this->button_save_image->setEnabled(false);
        }
    } else {
        this->button_open_path->setEnabled(false);
    }
}

/**
 * @brief slot_update_atom_label.
 */
void JobInfoWidget::slot_update_atom_label(int atom_id) {
    const Atom& atom = this->anaglyph_widget->get_structure()->get_atom(atom_id);
    this->label_selected_atom->setText(tr("Selected atom: %1 (#%2)").arg(AtomSettings::get().get_name_from_elnr(atom.atnr).c_str()).arg(atom_id+1));
}

/**
 * @brief slot_show_path_in_explorer_window.
 */
void JobInfoWidget::slot_show_path_in_explorer_window() {
    QString path = this->label_job_path->text();
    QFile file(path);
    if(file.exists()) {
        QFileInfo fileinfo(path);
        QDesktopServices::openUrl(fileinfo.absoluteDir().path());
    }
}

/**
 * @brief slot_save_image.
 */
void JobInfoWidget::slot_save_image() {
    QString imagepath = QFileInfo(this->label_job_path->text()).absoluteDir().path() + "/image.png";
    QFile imagefile(imagepath);
    if(imagefile.exists()) {
        QString filename = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                        tr("Images (*.png)"));
        if(!filename.isEmpty()) {
            imagefile.copy(filename);
        }
    }
}
