#ifndef JOBINFOWIDGET_H
#define JOBINFOWIDGET_H

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

    QPlainTextEdit* text_job_info;
    QLabel* label_image;
    QLabel* label_selected_atom;

    ThreadRenderImage* process_job_queue = nullptr;
    AnaglyphWidget* anaglyph_widget = nullptr;

public:
    explicit JobInfoWidget(QWidget *parent = nullptr);

    inline void set_process_job_queue_ptr(ThreadRenderImage* _process_job_queue) {
        this->process_job_queue = _process_job_queue;
    }

    inline AnaglyphWidget* get_anaglyph_widget() {
        return this->anaglyph_widget;
    }

    /**
     * @brief Rebuild structures based on AtomSettings data
     */
    void rebuild_structures();

signals:

public slots:
    void slot_update_job_info(int job_id);

private slots:
    void slot_update_atom_label(int atom_id);

    void slot_show_path_in_explorer_window();

    void slot_save_image();
};

#endif // JOBINFOWIDGET_H
