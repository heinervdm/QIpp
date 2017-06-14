#ifndef QIpp_H
#define QIpp_H

#include <QObject>
#include <QHash>
#include <QDebug>

#include <cups/cups.h>

class QTimer;
class QUrl;
class QIpp : public QObject
{
	Q_OBJECT

public:
	QIpp();
	QIpp(const char *host, const char *resource = "/ipp", const char *scheme ="ipp", int port = ippPort(), int interval = 1000);
	QIpp(const QUrl *url, int interval = 1000);
	virtual ~QIpp();

public slots:
	void Print(int fd, const char *jobname = "printjob") const;
	void Print(const char *filename) const;
	std::pair<QString,QString> getPrinterStatus();
	inline void setHost(const char *host) {
		m_host = QString(host);
	};
	inline void setScheme(const char *scheme) {
		m_scheme = QString(scheme);
	};
	inline void setResource(const char *resource) {
		m_resource = QString(resource);
	};
	inline void setPort(int port) {
		m_port = port;
	};
	void setStatusInterval(int interval);
	inline void printStatus(const QString state, const QString reason) {
		qDebug() << state << ":" << reason;
	}
	QString getExplanationForStateReason(const QString reason) const;
	QString getSeverityFromStateReason(QString reason) const;

signals:
	void printerStatusChanged(QString state, QString statereason);

private:
	void init();
	ipp_t *doRequest(ipp_t *request, int fd = -1, const char *filename = NULL) const;
	void handleJobResponse(ipp_t *response) const;
	QTimer *m_timer;
	QString m_state;  // printer state (idle, processing, stopped)
	QString m_statereason; // reason for state, see {{{m_states}}}
	int m_interval; // printer status retrival interval
	QString m_resource; // path part of the ipp URL
	int m_port; // ipp port, standard 631
	QString m_host; // printer host
	QString m_scheme; // ipp URL scheme (ipp, http, ...)
	QHash<QString,QString> *m_states; // https://tools.ietf.org/html/rfc2911#section-4.4.12

};

#endif // QIpp_H
