/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017  <copyright holder> <email>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef STATUSGUI_H
#define STATUSGUI_H

#include <QWidget>

class QPushButton;
class QLineEdit;
class QIpp;
class QTextEdit;
class StatusGui : public QWidget
{
    Q_OBJECT

public:
    StatusGui();
    ~StatusGui();

private slots:
	void connectPressed();
	void printerStatusChanged(QString,QString);

private:
	QPushButton *m_connectButton;
	QLineEdit *m_ipInput;
	bool m_connected;
	QIpp *m_ipp;
	QTextEdit *m_statusText;
};

#endif // STATUSGUI_H
