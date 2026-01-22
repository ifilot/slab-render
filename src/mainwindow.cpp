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
#include "mainwindow.h"

MainWindow::MainWindow(const std::shared_ptr<QStringList> _log_messages, QWidget *parent)
    : QMainWindow(parent),
    log_messages(_log_messages) {

    // log window
    this->log_window = std::make_unique<LogWindow>(this->log_messages);

    // set icon
    this->setWindowIcon(QIcon(QString(":/assets/icons/%1.ico").arg(PROGRAM_NAME_LC)));

    // top level container
    QHBoxLayout* layout_top = new QHBoxLayout();
    QSplitter* splitter = new QSplitter();
    this->setCentralWidget(splitter);

    // job queue container
    QGroupBox* container_left = new QGroupBox("Job queue");
    QVBoxLayout* layout_left = new QVBoxLayout();
    container_left->setLayout(layout_left);
    splitter->addWidget(container_left);

    // blender settings container
    QGroupBox* container_right = new QGroupBox("Blender settings");
    QVBoxLayout* layout_right = new QVBoxLayout();
    container_right->setLayout(layout_right);
    splitter->addWidget(container_right);

    // job info container
    QGroupBox* container_job_info = new QGroupBox("Job information");
    QVBoxLayout* layout_job_info = new QVBoxLayout();
    container_job_info->setLayout(layout_job_info);
    splitter->addWidget(container_job_info);
    this->widget_job_info = new JobInfoWidget();
    layout_job_info->addWidget(this->widget_job_info);

    // create container for list and a button to select files
    this->combobox_file_types = new QComboBox();
    for(const auto& strtype : this->GEOMETRY_FILETYPES) {
        combobox_file_types->addItem(strtype);
    }
    layout_left->addWidget(combobox_file_types);
    this->button_select_folder = new QPushButton("Select folder");
    layout_left->addWidget(button_select_folder);

    this->listview_items = new QListWidget();
    layout_left->addWidget(this->listview_items);

    this->executables = this->find_blender_executable();

    QWidget* container_buttons = new QWidget();
    QHBoxLayout* layout_buttons = new QHBoxLayout();
    container_buttons->setLayout(layout_buttons);
    layout_left->addWidget(container_buttons);
    this->button_parse_files = new QPushButton("Launch queue");
    layout_buttons->addWidget(this->button_parse_files);
    this->button_run_single_job = new QPushButton("Run single job");
    layout_buttons->addWidget(this->button_run_single_job);
    this->button_parse_files->setEnabled(false);
    this->button_cancel = new QPushButton("Cancel");
    layout_buttons->addWidget(this->button_cancel);
    this->button_cancel->setVisible(false);

    this->progress_bar = new QProgressBar();
    layout_left->addWidget(this->progress_bar);
    this->progress_bar->setEnabled(false);

    connect(this->button_select_folder, SIGNAL(released()), this, SLOT(slot_select_folder()));
    connect(this->button_cancel, SIGNAL(released()), this, SLOT(slot_cancel_queue()));
    connect(this->button_parse_files, SIGNAL(released()), this, SLOT(slot_parse_files()));
    connect(this->button_run_single_job, SIGNAL(released()), this, SLOT(slot_parse_single_job()));
    connect(this->listview_items, SIGNAL(currentRowChanged(int)), this->widget_job_info, SLOT(slot_update_job_info(int)));
    connect(this->listview_items, SIGNAL(currentRowChanged(int)), this->widget_job_info->get_anaglyph_widget(), SLOT(slot_load_structure(int)));
    connect(this->widget_job_info->get_pushbutton_angle_json(), SIGNAL(released()), this, SLOT(slot_add_object_angles()));
    connect(this->widget_job_info->get_pushbutton_insert_zoom_level(), SIGNAL(released()), this, SLOT(slot_set_zoom_level()));

    // set layout
    this->setMinimumWidth(1280);
    this->setMinimumHeight(768);
    this->setWindowTitle("Saucepan - the easy-ish Blender render engine");

    //  build blender settings interface
    build_blender_settings_panel(layout_right);

    // connect buttons for Blender settings panel
    connect(this->button_rebuild_structures, SIGNAL(released()), this, SLOT(slot_rebuild_structures()));

    this->build_dropdown_menu();
}

/**
  * @brief Set dropdown menu
  */
void MainWindow::build_dropdown_menu() {
    // create menu bar
    QMenuBar *menuBar = new QMenuBar;

    // add drop-down menus
    QMenu *menuFile = menuBar->addMenu(tr("&File"));
    QMenu *menuHelp = menuBar->addMenu(tr("&Help"));

    // open
    QAction *action_open = new QAction(menuFile);
    action_open->setText(tr("Open folder"));
    action_open->setShortcuts(QKeySequence::Open);
    menuFile->addAction(action_open);
    connect(action_open, &QAction::triggered, this, &MainWindow::slot_select_folder);

    // quit
    QAction *action_quit = new QAction(menuFile);
    action_quit->setText(tr("Quit"));
    action_quit->setShortcuts(QKeySequence::Quit);
    menuFile->addAction(action_quit);
    connect(action_quit, &QAction::triggered, this, &MainWindow::slot_exit);

    // debug log
    QAction *action_debug_log = new QAction(menuHelp);
    action_debug_log->setText(tr("Debug Log"));
    action_debug_log ->setShortcut(Qt::Key_F2);
    menuHelp->addAction(action_debug_log);
    connect(action_debug_log, &QAction::triggered, this, &MainWindow::slot_debug_log);

    // about
    QAction *action_about = new QAction(menuHelp);
    action_about->setText(tr("About"));
    menuHelp->addAction(action_about);
    action_about ->setShortcut(QKeySequence::WhatsThis);
    connect(action_about, &QAction::triggered, this, &MainWindow::slot_about);

    // build menu
    setMenuBar(menuBar);
}

void MainWindow::build_blender_settings_panel(QVBoxLayout* layout) {
    // custom icon for tooltips
    QIcon icon_info = QIcon(":/assets/icons/info.png");
    QPixmap pixmap_info = icon_info.pixmap(QSize(16, 16));

    //  build blender settings interface
    this->combobox_blender_executable = new QComboBox();
    layout->addWidget(this->combobox_blender_executable);
    auto blender_executables = this->find_blender_executable();
    blender_executables.sort();
    for(int i=0; i<blender_executables.count(); i++) {
        this->combobox_blender_executable->insertItem(i, blender_executables[i]);
    }
    this->combobox_blender_executable->setCurrentIndex(this->combobox_blender_executable->count()-1);

    this->button_probe_gpu = new QPushButton("Probe GPUs");
    layout->addWidget(this->button_probe_gpu);
    connect(this->button_probe_gpu, SIGNAL(released()), this, SLOT(slot_probe_gpu()));

    this->label_gpus = new QLabel();
    layout->addWidget(this->label_gpus);

    QWidget* container_settings = new QWidget();
    layout->addWidget(container_settings);
    QGridLayout* layout_blender_settings = new QGridLayout();
    container_settings->setLayout(layout_blender_settings);

    int rownr = 0;
    layout_blender_settings->addWidget(new QLabel("Ortho scale"), rownr, 0);
    this->combobox_ortho_scale = new QComboBox();
    layout_blender_settings->addWidget(this->combobox_ortho_scale, rownr, 1);
    this->combobox_ortho_scale->addItem("auto");
    this->combobox_ortho_scale->addItem("manual");

    rownr++;
    this->label_custom_ortho_scale = new QLabel("Custom ortho scale");
    layout_blender_settings->addWidget(this->label_custom_ortho_scale, rownr, 0);
    this->spinbox_custom_ortho_scale = new QDoubleSpinBox();
    layout_blender_settings->addWidget(this->spinbox_custom_ortho_scale, rownr, 1);
    this->spinbox_custom_ortho_scale->setMinimum(10.0);
    this->spinbox_custom_ortho_scale->setMaximum(1000.0);
    this->label_custom_ortho_scale->setVisible(false);
    this->spinbox_custom_ortho_scale->setVisible(false);
    connect(this->combobox_ortho_scale, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_change_ortho_scale(int)));

    rownr++;
    layout_blender_settings->addWidget(new QLabel("Camera direction"), rownr, 0);
    this->combobox_camera_direction = new QComboBox();
    layout_blender_settings->addWidget(this->combobox_camera_direction, rownr, 1);
    this->combobox_camera_direction->addItem("Z+");
    this->combobox_camera_direction->addItem("Z-");
    this->combobox_camera_direction->addItem("Y+");
    this->combobox_camera_direction->addItem("Y-");
    this->combobox_camera_direction->addItem("X+");
    this->combobox_camera_direction->addItem("X-");

    // whether to hide the axes
    rownr++;
    layout_blender_settings->addWidget(new QLabel("Hide axes"), rownr, 0);
    this->checkbox_axes = new QCheckBox();
    layout_blender_settings->addWidget(this->checkbox_axes, rownr, 1);
    this->checkbox_axes->setChecked(true);

    // whether to show the unit cell
    rownr++;
    layout_blender_settings->addWidget(new QLabel("Show unitcell"), rownr, 0);
    this->checkbox_unitcell = new QCheckBox();
    layout_blender_settings->addWidget(this->checkbox_unitcell, rownr, 1);

    // whether to expand the system in the xy directions
    rownr++;
    layout_blender_settings->addWidget(new QLabel("Expansion"), rownr, 0);
    this->checkbox_expansion = new QCheckBox();
    layout_blender_settings->addWidget(this->checkbox_expansion, rownr, 1);

    // canvas reoslution in x direction
    rownr++;
    layout_blender_settings->addWidget(new QLabel("Resolution x"), rownr, 0);
    this->spinbox_resolution_x = new QSpinBox();
    layout_blender_settings->addWidget(this->spinbox_resolution_x, rownr, 1);
    this->spinbox_resolution_x->setMinimum(128);
    this->spinbox_resolution_x->setMaximum(2048);
    this->spinbox_resolution_x->setValue(512);

    // canvas resolution in y direction
    rownr++;
    layout_blender_settings->addWidget(new QLabel("Resolution y"), rownr, 0);
    this->spinbox_resolution_y = new QSpinBox();
    layout_blender_settings->addWidget(this->spinbox_resolution_y, rownr, 1);
    this->spinbox_resolution_y->setMinimum(128);
    this->spinbox_resolution_y->setMaximum(2048);
    this->spinbox_resolution_y->setValue(512);

    // tile size in x direction
    rownr++;
    layout_blender_settings->addWidget(new QLabel("Tile x"), rownr, 0);
    this->spinbox_tile_x = new QSpinBox();
    layout_blender_settings->addWidget(this->spinbox_tile_x, rownr, 1);
    this->spinbox_tile_x->setMinimum(128);
    this->spinbox_tile_x->setMaximum(2048);
    this->spinbox_tile_x->setValue(256);

    // tile size in y direction
    rownr++;
    layout_blender_settings->addWidget(new QLabel("Tile y"), rownr, 0);
    this->spinbox_tile_y = new QSpinBox();
    layout_blender_settings->addWidget(this->spinbox_tile_y, rownr, 1);
    this->spinbox_tile_y->setMinimum(128);
    this->spinbox_tile_y->setMaximum(2048);
    this->spinbox_tile_y->setValue(256);

    // number of samples
    rownr++;
    layout_blender_settings->addWidget(new QLabel("Samples"), rownr, 0);
    this->spinbox_samples = new QSpinBox();
    layout_blender_settings->addWidget(this->spinbox_samples, rownr, 1);
    this->spinbox_samples->setMinimum(128);
    this->spinbox_samples->setMaximum(2048);
    this->spinbox_samples->setValue(128);

    // number of subdivions for the spheres
    rownr++;
    layout_blender_settings->addWidget(new QLabel("Number of subdivions"), rownr, 0);
    this->spinbox_nsubdiv = new QSpinBox();
    layout_blender_settings->addWidget(this->spinbox_nsubdiv, rownr, 1);
    this->spinbox_nsubdiv->setMinimum(1);
    this->spinbox_nsubdiv->setMaximum(5);
    this->spinbox_nsubdiv->setValue(4);

    rownr++;
    layout_blender_settings->addWidget(new QLabel("Material for atoms"), rownr, 0);
    this->combobox_atom_material = new QComboBox();
    this->combobox_atom_material->addItem("specular");
    this->combobox_atom_material->addItem("soft");
    layout_blender_settings->addWidget(this->combobox_atom_material, rownr, 1);

    rownr++;
    layout_blender_settings->addWidget(new QLabel("Material for bonds"), rownr, 0);
    this->combobox_bond_material = new QComboBox();
    this->combobox_bond_material->addItem("specular");
    this->combobox_bond_material->addItem("soft");
    layout_blender_settings->addWidget(this->combobox_bond_material, rownr, 1);
    this->combobox_bond_material->setCurrentIndex(1);

    rownr++;
    layout_blender_settings->addWidget(new QLabel("Custom settings (json)"), rownr, 0);
    QLabel* tooltip_info = new QLabel;
    tooltip_info->setPixmap(pixmap_info);
    tooltip_info->setToolTip(this->fetch_tooltip_text("custom_json_example"));
    tooltip_info->setFixedWidth(20);
    layout_blender_settings->addWidget(tooltip_info, rownr, 2);
    rownr++;
    this->plaintext_modding = new QPlainTextEdit();
    layout_blender_settings->addWidget(this->plaintext_modding, rownr, 0, 1, 2);
    this->plaintext_modding->setPlainText("\"atom_colors\": [\n\n],\n\"atom_radii\": [\n\n],\n\"bond_distances\": [\n\n],");
    connect(this->plaintext_modding, SIGNAL(textChanged()), this, SLOT(slot_check_valid_json()));
    rownr++;
    this->label_valid_json = new QLabel("JSON validation pass");
    this->label_valid_json->setStyleSheet("QLabel { background-color : green; color : white; }");
    layout_blender_settings->addWidget(this->label_valid_json, rownr, 0, 1, 2);

    // rebuild images
    rownr++;
    this->button_rebuild_structures = new QPushButton("Rebuild structures");
    layout_blender_settings->addWidget(this->button_rebuild_structures, rownr, 0);

    QFrame* frame = new QFrame();
    layout->addWidget(frame);
    frame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

MainWindow::~MainWindow()
{
}

QStringList MainWindow::find_blender_executable() {
    QString path = QDir::cleanPath("C:/Program Files/Blender Foundation");
    QDirIterator it(path, {"blender.exe"}, QDir::NoFilter | QDir::Executable | QDir::Files, QDirIterator::Subdirectories);
    QStringList files;
    do {
        files << it.next();
    } while(it.hasNext());

    return files;
}

/**
 * @brief Find files with a specific file name
 */
QStringList MainWindow::find_files(const QString& path, const QStringList& filenames) {
    qDebug() << "Finding files with pattern: " << filenames;
    QString p = QDir::cleanPath(path);
    QDirIterator it(p, filenames, QDir::Files, QDirIterator::Subdirectories);
    QStringList files;
    do {
        files << it.next();
    } while(it.hasNext());

    return files;
}

QString MainWindow::fetch_tooltip_text(const QString& filename) {
    QFile file(":/assets/tooltips/" + filename + ".txt");
    if(file.open(QIODevice::ReadOnly)) {
        return file.readAll();
    }
    return {};
}

void MainWindow::slot_parse_files() {
    // disable all buttons
    this->button_parse_files->setEnabled(false);
    this->button_select_folder->setEnabled(false);

    // enable cancellation button
    this->button_cancel->setVisible(true);
    this->button_cancel->setEnabled(true);

    // disable run single job button
    this->button_run_single_job->setEnabled(false);

    // set progress bar
    this->progress_bar->setMaximum(this->listview_items->count());

    // set queue job
    this->process_job_queue->set_single_job_id(-1);

    // connect signals and slots
    connect(process_job_queue.get(), SIGNAL(signal_job_done(int)), this, SLOT(slot_job_done(int)));
    connect(process_job_queue.get(), SIGNAL(signal_job_start(int)), this, SLOT(slot_job_start(int)));
    connect(process_job_queue.get(), SIGNAL(signal_queue_done()), this, SLOT(slot_queue_done()));
    connect(process_job_queue.get(), SIGNAL(signal_queue_cancelled()), this, SLOT(slot_queue_cancelled()));

    // collect blender settings
    QMap<QString, QVariant> parameters;
    parameters.insert("ortho_scale", QVariant(this->combobox_ortho_scale->currentText()));
    parameters.insert("ortho_custom_scale", QVariant(this->spinbox_custom_ortho_scale->value()));
    parameters.insert("camera_direction", QVariant(this->combobox_camera_direction->currentText()));
    parameters.insert("show_unitcell", QVariant(this->checkbox_unitcell->isChecked()));
    parameters.insert("expansion", QVariant(this->checkbox_expansion->isChecked()));
    parameters.insert("hide_axes", QVariant(this->checkbox_axes->isChecked()));
    parameters.insert("resolution_x", QVariant(this->spinbox_resolution_x->value()));
    parameters.insert("resolution_y", QVariant(this->spinbox_resolution_y->value()));
    parameters.insert("tile_x", QVariant(this->spinbox_tile_x->value()));
    parameters.insert("tile_y", QVariant(this->spinbox_tile_y->value()));
    parameters.insert("samples", QVariant(this->spinbox_samples->value()));
    parameters.insert("nsubdiv", QVariant(this->spinbox_nsubdiv->value()));
    parameters.insert("atmat", QVariant(this->combobox_atom_material->currentText()));
    parameters.insert("bondmat", QVariant(this->combobox_bond_material->currentText()));
    parameters.insert("custom_json", QVariant(this->plaintext_modding->toPlainText()));

    // set icon when jobs are in queue
    static const QIcon icon(":/assets/icons/queue.png");
    for(int i=0; i<this->listview_items->count(); i++) {
        this->listview_items->item(i)->setIcon(icon);
    }

    // launch queue
    process_job_queue->set_parameters(parameters);
    process_job_queue->start();
}

void MainWindow::slot_parse_single_job() {
    // disable all buttons
    this->button_parse_files->setEnabled(false);
    this->button_select_folder->setEnabled(false);

    // enable cancellation button
    this->button_cancel->setVisible(true);
    this->button_cancel->setEnabled(true);

    // disable run single job button
    this->button_run_single_job->setEnabled(false);

    // set progress bar
    this->progress_bar->setMaximum(1);

    int jobid = this->listview_items->currentRow();
    this->process_job_queue->set_single_job_id(jobid);

    // connect signals and slots
    connect(process_job_queue.get(), SIGNAL(signal_job_done(int)), this, SLOT(slot_job_done(int)));
    connect(process_job_queue.get(), SIGNAL(signal_job_start(int)), this, SLOT(slot_job_start(int)));
    connect(process_job_queue.get(), SIGNAL(signal_queue_done()), this, SLOT(slot_queue_done()));
    connect(process_job_queue.get(), SIGNAL(signal_queue_cancelled()), this, SLOT(slot_queue_cancelled()));

    // collect blender settings
    QMap<QString, QVariant> parameters;
    parameters.insert("ortho_scale", QVariant(this->combobox_ortho_scale->currentText()));
    parameters.insert("ortho_custom_scale", QVariant(this->spinbox_custom_ortho_scale->value()));
    parameters.insert("camera_direction", QVariant(this->combobox_camera_direction->currentText()));
    parameters.insert("show_unitcell", QVariant(this->checkbox_unitcell->isChecked()));
    parameters.insert("expansion", QVariant(this->checkbox_expansion->isChecked()));
    parameters.insert("hide_axes", QVariant(this->checkbox_axes->isChecked()));
    parameters.insert("resolution_x", QVariant(this->spinbox_resolution_x->value()));
    parameters.insert("resolution_y", QVariant(this->spinbox_resolution_y->value()));
    parameters.insert("tile_x", QVariant(this->spinbox_tile_x->value()));
    parameters.insert("tile_y", QVariant(this->spinbox_tile_y->value()));
    parameters.insert("samples", QVariant(this->spinbox_samples->value()));
    parameters.insert("nsubdiv", QVariant(this->spinbox_nsubdiv->value()));
    parameters.insert("atmat", QVariant(this->combobox_atom_material->currentText()));
    parameters.insert("bondmat", QVariant(this->combobox_bond_material->currentText()));
    parameters.insert("custom_json", QVariant(this->plaintext_modding->toPlainText()));

    // set icon when jobs are in queue
    static const QIcon icon(":/assets/icons/queue.png");
    this->listview_items->item(jobid)->setIcon(icon);

    // launch queue
    process_job_queue->set_parameters(parameters);
    process_job_queue->start();
}

void MainWindow::slot_check_valid_json() {
    std::string json_string = "{" + this->plaintext_modding->toPlainText().toStdString() + "}";
    try {
        json::jobject result = json::jobject::parse(json_string);
        this->label_valid_json->setText("JSON validation pass");
        this->label_valid_json->setStyleSheet("QLabel { background-color : green; color : white; }");
    } catch(const std::exception& e) {
        this->label_valid_json->setText("Invalid JSON detected");
        this->label_valid_json->setStyleSheet("QLabel { background-color : red; color : white; }");
    }
}

void MainWindow::slot_select_folder() {
    qDebug() << "Opening dialog";
    auto path = QFileDialog::getExistingDirectory(0, ("Select data folder"), QDir::currentPath());
    if(path.isEmpty()) {
        return;
    } else {
        qDebug() << "Clearing job queue";
        this->process_job_queue.reset();
        this->job_status.clear();
        this->widget_job_info->set_process_job_queue_ptr(nullptr);

        qDebug() << "Clearing list view";
        this->listview_items->clear();
    }

    QStringList files;
    if(this->combobox_file_types->currentText() == this->GEOMETRY_FILETYPES[0]) { // VASP CONTCAR
        files = this->find_files(path, {"POSCAR*","CONTCAR*"});
    } else if(this->combobox_file_types->currentText() == this->GEOMETRY_FILETYPES[1]) { // ADF LOGFILES
        files = this->find_files(path, {"logfile"});
    } else if(this->combobox_file_types->currentText() == this->GEOMETRY_FILETYPES[2]) { // Gaussian log files
        files = this->find_files(path, {"*.LOG","*.log"});
    } else {
        throw std::runtime_error("Invalid selection. Terminating program.");
    }

    // icon for file to be rendered
    static const QIcon icon(":/assets/icons/space_invader.png");

    int iterator = 0;
    for(const QString& file : files) {
        auto item = new QListWidgetItem();
        item->setIcon(icon);
        item->setText(file);
        this->listview_items->insertItem(iterator, item);
        this->job_status.push_back(JOB_QUEUED);
        iterator++;
    }

    this->widget_job_info->get_anaglyph_widget()->set_structure_paths(files);

    this->button_parse_files->setEnabled(true);

    // build queue object
    this->process_job_queue = std::make_unique<ThreadRenderImage>();
    this->widget_job_info->set_process_job_queue_ptr(this->process_job_queue.get());
    process_job_queue->set_files(files);
    process_job_queue->set_executable(this->combobox_blender_executable->currentText());
}

void MainWindow::slot_job_start(int jobid) {
    this->progress_bar->setValue(jobid+1);

    static const QIcon icon(":/assets/icons/processor.png");

    this->listview_items->item(jobid)->setIcon(icon);
    this->job_status[jobid] = JOB_RUNNING;
}

void MainWindow::slot_job_done(int jobid) {
    this->progress_bar->setValue(jobid+1);

    static const QIcon icon(":/assets/icons/image.png");

    this->listview_items->item(jobid)->setIcon(icon);
    double ptime = this->process_job_queue->get_process_time(jobid);
    QString newtext = this->listview_items->item(jobid)->text() + tr(" (%1 sec.)").arg(ptime);
    this->listview_items->item(jobid)->setText(newtext);
    this->job_status[jobid] = JOB_COMPLETED;
    this->listview_items->setCurrentRow(jobid);
    this->widget_job_info->slot_update_job_info(jobid);
}

void MainWindow::slot_queue_done() {
    this->button_parse_files->setEnabled(true);
    this->button_select_folder->setEnabled(true);
    this->button_cancel->setVisible(false);
    this->button_run_single_job->setEnabled(true);
}

void MainWindow::slot_probe_gpu() {
    qDebug() << "Probe GPUs";
    this->label_gpus->clear();
    QTemporaryDir dir;
    dir.setAutoRemove(false); // do not immediately remove
    if(dir.isValid()) {
        // write Blender axes template file
        QFile blenderfile(":/assets/blender/axes_template.blend");
        if(!blenderfile.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("Could not open blender file from assets.");
        }
        blenderfile.copy(dir.path() + "/axes_template.blend");

        // write Python file containing Blender instructions
        QFile pythonfile(":/assets/blender/probe_cards.py");
        if(!pythonfile.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("Could not open Python file from assets.");
        }
        pythonfile.copy(dir.path() + "/probe_cards.py");

        QStringList arguments = {"-b", "-P", "probe_cards.py"};
        QProcess* blender_process = new QProcess();
        blender_process->setProgram(this->combobox_blender_executable->currentText());
        blender_process->setArguments(arguments);
        blender_process->setProcessChannelMode(QProcess::SeparateChannels);
        blender_process->setWorkingDirectory(dir.path());
        blender_process->start();
        if(blender_process->waitForFinished(1 * 60 * 1000)) { // max one minute
            auto lines = blender_process->readAll().split('\n');
            for(const QString& line : lines) {
                if(line.contains("CyclesDeviceSettings") && line.contains("NVIDIA", Qt::CaseInsensitive)) {
                    QString substr = line.split("CyclesDeviceSettings(\"")[1].split("\") at ")[0];
                    if(this->label_gpus->text().count()) {
                        this->label_gpus->setText(this->label_gpus->text() + "\n" + substr);
                    } else {
                        this->label_gpus->setText(substr);
                    }
                }
            }
        }

    } else {
        throw std::runtime_error("Invalid path");
    }
}

void MainWindow::slot_change_ortho_scale(int item_id) {
    if(item_id > 0) {
        this->label_custom_ortho_scale->setVisible(true);
        this->spinbox_custom_ortho_scale->setVisible(true);
    } else {
        this->label_custom_ortho_scale->setVisible(false);
        this->spinbox_custom_ortho_scale->setVisible(false);
    }
}

void MainWindow::slot_add_object_angles() {
    QVector3D camera = this->widget_job_info->get_anaglyph_widget()->get_euler_angles();
    QTextCursor new_cursor = this->plaintext_modding->textCursor();
    new_cursor.movePosition(QTextCursor::End);
    this->plaintext_modding->setTextCursor(new_cursor);
    this->plaintext_modding->insertPlainText(tr("\n\"object_euler\": \"%1/%2/%3\",").arg(camera[0], 0, 'f', 2).arg(camera[1], 0, 'f', 2).arg(camera[2], 0, 'f', 2));
}

void MainWindow::slot_set_zoom_level() {
    this->combobox_ortho_scale->setCurrentIndex(1);
    this->spinbox_custom_ortho_scale->setValue(this->widget_job_info->get_anaglyph_widget()->get_camera_position()[2]);
}

void MainWindow::slot_cancel_queue() {
    if(this->process_job_queue && this->process_job_queue.get()->isRunning()) {
        qDebug() << "Requesting interruption of queue, wait until current job is finished...";
        this->process_job_queue->requestInterruption();
        this->button_cancel->setEnabled(false);
    }
}

void MainWindow::slot_queue_cancelled() {
    qDebug() << "Job cancellation received, updating status.";
    this->button_parse_files->setEnabled(true);
    this->button_select_folder->setEnabled(true);
    this->button_run_single_job->setEnabled(true);
    this->button_cancel->setVisible(false);
    this->progress_bar->reset();
    this->progress_bar->setValue(0);

    // set icon when jobs are in queue
    static const QIcon icon(":/assets/icons/cancelled.png");
    for(int i=0; i<this->listview_items->count(); i++) {
        if(this->job_status[i] < JOB_COMPLETED) {
            this->job_status[i] = JOB_CANCELLED;
            this->listview_items->item(i)->setIcon(icon);
        }
    }
}

void MainWindow::slot_exit() {
    QApplication::quit();
}

void MainWindow::slot_debug_log() {
    this->log_window->show();
}

void MainWindow::slot_about() {
    QMessageBox message_box;
        //message_box.setStyleSheet("QLabel{min-width: 250px; font-weight: normal;}");
        message_box.setText(PROGRAM_NAME
                            " version "
                            PROGRAM_VERSION
                            ".\n\nAuthor:\nIvo Filot <i.a.w.filot@tue.nl>\n\n"
                            PROGRAM_NAME " is licensed under the GPLv3 license.\n\n"
                            PROGRAM_NAME " is dynamically linked to Qt, which is licensed under LGPLv3.\n");
        message_box.setIcon(QMessageBox::Information);
        message_box.setWindowTitle("About " + tr(PROGRAM_NAME));
        message_box.setWindowIcon(QIcon(QString(":/assets/icons/%1.ico").arg(PROGRAM_NAME_LC)));
        message_box.exec();
}

void MainWindow::slot_rebuild_structures() {
    // overwrite AtomSettings object
    AtomSettings::get().overwrite(this->plaintext_modding->toPlainText().toStdString());

    // instruct jobinfowidget to rebuild structures
    this->widget_job_info->rebuild_structures();
}
