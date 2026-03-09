// SPDX-License-Identifier: LGPL-3.0-or-later
// SlabRender
// Author: Ivo Filot <ivo@ivofilot.nl>


#pragma once

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPlainTextEdit>
#include <QDebug>
#include <QTimer>
#include <QIcon>

class LogWindow : public QWidget {

Q_OBJECT

private:
    std::shared_ptr<QStringList> log_messages;
    QPlainTextEdit* text_box;
    int linesread = 0;

public:
    LogWindow(){}

   /**
    * @brief LogWindow.
    */
    LogWindow(const std::shared_ptr<QStringList>& _log_messages);

private slots:
    /**
     * @brief update_log.
     */
    void update_log();
};
