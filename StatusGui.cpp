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

#include "StatusGui.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QUrl>
#include <QTextEdit>

#include "QIpp.h"

StatusGui::StatusGui() : QWidget(), m_connected(false) {
	QGridLayout *l = new QGridLayout;
	setLayout(l);
	QLabel *iplabel = new QLabel(tr("Printer URI:"));
	m_ipInput = new QLineEdit("ipp://10.132.139.33:631/ipp");
	m_connectButton = new QPushButton(tr("Connect"));
	l->addWidget(iplabel,1,1,Qt::AlignRight);
	l->addWidget(m_ipInput,1,2);
	l->addWidget(m_connectButton,1,3);
	connect(m_connectButton, SIGNAL(pressed()), this, SLOT(connectPressed()));
	m_statusText = new QTextEdit;
	l->addWidget(m_statusText,2,1,1,3);
}

StatusGui::~StatusGui() {
}

void StatusGui::connectPressed() {
	if (m_connected) {
		delete m_ipp;
		m_connectButton->setText(tr("Connect"));
		m_connected = false;
		m_statusText->setText("");
	} else {
		QUrl url = QUrl::fromUserInput(m_ipInput->text());
		m_ipp = new QIpp(&url);
		connect(m_ipp, SIGNAL(printerStatusChanged(QString,QString)), this, SLOT(printerStatusChanged(QString,QString)));
		m_connectButton->setText(tr("Disconnect"));
		m_connected = true;
		std::pair<QString,QString> status = m_ipp->getPrinterStatus();
		printerStatusChanged(status.first, status.second);
	}
}

void StatusGui::printerStatusChanged (QString state, QString statereason) {
	m_statusText->setText(tr("State: ")+state+"\n"+tr("Description: ")+m_ipp->getExplanationForStateReason(statereason)+"\n"+tr("Severity: ")+m_ipp->getSeverityFromStateReason(statereason));
}


#include "StatusGui.moc"
