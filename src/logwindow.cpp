// SPDX-License-Identifier: LGPL-3.0-or-later
// SlabRender
// Author: Ivo Filot <ivo@ivofilot.nl>


#include "logwindow.h"
#include "config.h"

LogWindow::LogWindow(const std::shared_ptr<QStringList>& _log_messages) :
    log_messages(_log_messages)
{
    qDebug() << "Spawning Debug log window";
    this->setWindowIcon(QIcon(QString(":/assets/icons/%1.ico").arg(PROGRAM_NAME_LC)));
    this->setWindowTitle("Debug log");

    qDebug() << "Building log window layout";
    QVBoxLayout* layout = new QVBoxLayout;
    this->setLayout(layout);
    QScrollArea* scroll_area = new QScrollArea(this);
    layout->addWidget(scroll_area);
    QVBoxLayout* layout_scrollarea = new QVBoxLayout;
    scroll_area->setLayout(layout_scrollarea);
    this->text_box = new QPlainTextEdit(this);
    layout_scrollarea->addWidget(this->text_box);
    this->text_box->setReadOnly(true);
    this->text_box->setOverwriteMode(false);

    for(const auto& line : *this->log_messages.get()) {
        this->text_box->appendPlainText(line);
    }
    this->linesread = this->log_messages->size();

    this->setGeometry(640, 480, 640, 480);
    this->hide();

    // setup timer that checks every second whether
    // the log messages have been updated
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update_log()));
    timer->start(1000);
}

/**
 * @brief update_log.
 */
void LogWindow::update_log() {
    int newsize = this->log_messages->size();
    for(int i=this->linesread; i<newsize; i++) {
        this->text_box->appendPlainText(this->log_messages->at(i));
    }
    this->linesread = newsize;
}
