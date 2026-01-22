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

#ifndef LOGWINDOW_H
#define LOGWINDOW_H

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

    LogWindow(const std::shared_ptr<QStringList>& _log_messages);

private slots:
    void update_log();
};

#endif // LOGWINDOW_H
