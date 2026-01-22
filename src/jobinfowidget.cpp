#include "jobinfowidget.h"

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

    QWidget* container_angle = new QWidget();
    QHBoxLayout* layout_angle = new QHBoxLayout();
    container_angle->setLayout(layout_angle);
    anaglyph_container->layout()->addWidget(container_angle);
    this->label_camera_euler = new QLabel("Object Euler angles");
    layout_angle->addWidget(this->label_camera_euler);
    this->button_insert_angle_json = new QPushButton("<< Insert unitcell orientation");
    layout_angle->addWidget(this->button_insert_angle_json);

    container_angle = new QWidget();
    QHBoxLayout* layout_zoom = new QHBoxLayout();
    container_angle->setLayout(layout_zoom);
    anaglyph_container->layout()->addWidget(container_angle);
    this->label_zoom_level = new QLabel("Zoom level");
    layout_zoom->addWidget(this->label_zoom_level);
    this->button_insert_zoom_level = new QPushButton("<< Insert zoom level");
    layout_zoom->addWidget(this->button_insert_zoom_level);

    this->label_selected_atom = new QLabel("Atom selection");
    anaglyph_container->layout()->addWidget(this->label_selected_atom);

    connect(this->anaglyph_widget, SIGNAL(signal_atom_selected(int)), this, SLOT(slot_update_atom_label(int)));
    connect(this->anaglyph_widget, SIGNAL(signal_object_angles()), this, SLOT(slot_update_camera()));
    connect(this->anaglyph_widget, SIGNAL(signal_zoom_level()), this, SLOT(slot_update_zoom_level()));
}

/**
 * @brief Rebuild structures based on AtomSettings data
 */
void JobInfoWidget::rebuild_structures() {
    qDebug() << "Rebuilding structures based on new JSON data";
    this->anaglyph_widget->get_structure()->update();
    this->anaglyph_widget->update();
}

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

void JobInfoWidget::slot_update_atom_label(int atom_id) {
    const Atom& atom = this->anaglyph_widget->get_structure()->get_atom(atom_id);
    this->label_selected_atom->setText(tr("Selected atom: %1 (#%2)").arg(AtomSettings::get().get_name_from_elnr(atom.atnr).c_str()).arg(atom_id+1));
}

void JobInfoWidget::slot_update_camera() {
    QVector3D camera = this->anaglyph_widget->get_euler_angles();
    this->label_camera_euler->setText(tr("X=%1° Y=%2° Z=%3°").arg(camera[0], 0, 'f', 2).arg(camera[1], 0, 'f', 2).arg(camera[2], 0, 'f', 2));
}

void JobInfoWidget::slot_update_zoom_level() {
    this->label_zoom_level->setText(tr("Orthographic scale: %1").arg(this->anaglyph_widget->get_camera_position()[2]));
}

void JobInfoWidget::slot_show_path_in_explorer_window() {
    QString path = this->label_job_path->text();
    QFile file(path);
    if(file.exists()) {
        QFileInfo fileinfo(path);
        QDesktopServices::openUrl(fileinfo.absoluteDir().path());
    }
}

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
