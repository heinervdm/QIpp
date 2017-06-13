#include "QIpp.h"
#include <QTimer>
#include <QUrl>
#include <QStringList>

#include "config.h"

QIpp::QIpp() : m_interval (1000), m_port (ippPort()), m_resource ("/ipp"), m_scheme ("ipp"), m_host (cupsServer()) {
	init();
}

QIpp::QIpp (const char* host, const char* resource, const char* scheme, int port, int interval) : QObject(), m_host (host), m_resource (resource), m_scheme (scheme), m_port (port), m_interval (interval) {
	init();
}

QIpp::QIpp (const QUrl* url, int interval) : QObject(), m_interval (interval) {
	if (url && url->isValid()) {
		m_scheme = url->scheme();
		m_host = url->host();
		m_port = url->port (ippPort());
		m_resource = url->path();
	} else {
		m_scheme = "ipp";
		m_host = cupsServer();
		m_port = ippPort();
		m_resource = "/ipp";
	}

	init();
}


void QIpp::init() {
	// https://tools.ietf.org/html/rfc2911#section-4.4.12
	m_states = new QHash<QString,QString>;
	m_states->insert ("other",tr ("The device has detected an error other than one listed in this document."));
	m_states->insert ("none",tr ("There are not reasons. This state reason is semantically"
	                             "equivalent to \"printer-state-reasons\" without any value and"
	                             "MUST be used, since the 1setOf attribute syntax requires at"
	                             "least one value."));
	m_states->insert ("media-needed",tr ("A tray has run out of media."));
	m_states->insert ("media-jam",tr ("The device has a media jam."));
	m_states->insert ("moving-to-paused",tr ("Someone has paused the Printer object using"
	                  "the Pause-Printer operation (see section 3.2.7) or other means,"
	                  "but the device(s) are taking an appreciable time to stop."
	                  "Later, when all output has stopped, the \"printer-state\" becomes"
	                  "'stopped', and the 'paused' value replaces the 'moving-to-"
	                  "paused' value in the \"printer-state-reasons\" attribute.  This"
	                  "value MUST be supported, if the Pause-Printer operation is"
	                  "supported and the implementation takes significant time to"
	                  "pause a device in certain circumstances."));
	m_states->insert ("paused",tr ("Someone has paused the Printer object using the Pause-"
	                               "Printer operation (see section 3.2.7) or other means and the"
	                               "Printer object's \"printer-state\" is 'stopped'.  In this state,"
	                               "a Printer MUST NOT produce printed output, but it MUST perform"
	                               "other operations requested by a client.  If a Printer had been"
	                               "printing a job when the Printer was paused, the Printer MUST"
	                               "resume printing that job when the Printer is no longer paused"
	                               "and leave no evidence in the printed output of such a pause."
	                               "This value MUST be supported, if the Pause-Printer operation is"
	                               "supported."));
	m_states->insert ("shutdown",tr ("Someone has removed a Printer object from service, and"
	                                 "the device may be powered down or physically removed.  In this"
	                                 "state, a Printer object MUST NOT produce printed output, and"
	                                 "unless the Printer object is realized by a print server that is"
	                                 "still active, the Printer object MUST perform no other"
	                                 "operations requested by a client, including returning this"
	                                 "value. If a Printer object had been printing a job when it was"
	                                 "shutdown, the Printer NEED NOT resume printing that job when"
	                                 "the Printer is no longer shutdown. If the Printer resumes"
	                                 "printing such a job, it may leave evidence in the printed"
	                                 "output of such a shutdown, e.g. the part printed before the"
	                                 "shutdown may be printed a second time after the shutdown."));
	m_states->insert ("connecting-to-device",tr ("The Printer object has scheduled a job on"
	                  "the output device and is in the process of connecting to a"
	                  "shared network output device (and might not be able to actually"
	                  "start printing the job for an arbitrarily long time depending"
	                  "on the usage of the output device by other servers on the"
	                  "network)."));
	m_states->insert ("timed-out",tr ("The server was able to connect to the output device"
	                                  "(or is always connected), but was unable to get a response from"
	                                  "the output device."));
	m_states->insert ("stopping",tr ("The Printer object is in the process of stopping the"
	                                 "device and will be stopped in a while. When the device is"
	                                 "stopped, the Printer object will change the Printer object's"
	                                 "state to 'stopped'.  The 'stopping-warning' reason is never an"
	                                 "error, even for a Printer with a single output device.  When an"
	                                 "output-device ceases accepting jobs, the Printer will have this"
	                                 "reason while the output device completes printing."));
	m_states->insert ("stopped-partly",tr ("When a Printer object controls more than one"
	                                       "output device, this reason indicates that one or more output"
	                                       "devices are stopped.  If the reason is a report, fewer than"
	                                       "half of the output devices are stopped.  If the reason is a"
	                                       "warning, fewer than all of the output devices are stopped."));
	m_states->insert ("toner-low",tr ("The device is low on toner."));
	m_states->insert ("toner-empty",tr ("The device is out of toner."));
	m_states->insert ("spool-area-full",tr ("The limit of persistent storage allocated for"
	                                        "spooling has been reached.  The Printer is temporarily unable"
	                                        "to accept more jobs.  The Printer will remove this value when"
	                                        "it is able to accept more jobs.  This value SHOULD be used by a"
	                                        "non-spooling Printer that only accepts one or a small number"
	                                        "jobs at a time or a spooling Printer that has filled the spool"
	                                        "space."));
	m_states->insert ("cover-open",tr ("One or more covers on the device are open."));
	m_states->insert ("interlock-open",tr ("One or more interlock devices on the printer are unlocked."));
	m_states->insert ("door-open",tr ("One or more doors on the device are open."));
	m_states->insert ("input-tray-missing",tr ("One or more input trays are not in the device."));
	m_states->insert ("media-low",tr ("At least one input tray is low on media."));
	m_states->insert ("media-empty",tr ("At least one input tray is empty."));
	m_states->insert ("output-tray-missing",tr ("One or more output trays are not in the device"));
	m_states->insert ("output-area-almost-full",tr ("One or more output area is almost full (e.g. tray, stacker, collator)."));
	m_states->insert ("output-area-full",tr ("One or more output area is full. (e.g. tray, stacker, collator)"));
	m_states->insert ("marker-supply-low",tr ("The device is low on at least one marker supply.  (e.g. toner, ink, ribbon)"));
	m_states->insert ("marker-supply-empty",tr ("The device is out of at least one marker supply. (e.g. toner, ink, ribbon)"));
	m_states->insert ("marker-waste-almost-full",tr ("The device marker supply waste receptacle is almost full."));
	m_states->insert ("marker-waste-full",tr ("The device marker supply waste receptacle is full."));
	m_states->insert ("fuser-over-temp",tr ("The fuser temperature is above normal."));
	m_states->insert ("fuser-under-temp",tr ("The fuser temperature is below normal."));
	m_states->insert ("opc-near-eol",tr ("The optical photo conductor is near end of life."));
	m_states->insert ("opc-life-over",tr ("The optical photo conductor is no longer functioning."));
	m_states->insert ("developer-low",tr ("The device is low on developer."));
	m_states->insert ("developer-empty",tr ("The device is out of developer."));
	m_states->insert ("interpreter-resource-unavailable",tr ("An interpreter resource is unavailable (i.e. font, form)"));

	m_timer = new QTimer;
	connect (m_timer, SIGNAL (timeout()), this, SLOT (getPrinterStatus()));
	m_timer->start (m_interval);
	getPrinterStatus();
}

QIpp::~QIpp() {
	if (m_timer) {
		if (m_timer->isActive()) {
			m_timer->stop();
		}
		delete m_timer;
	}
}

ipp_t* QIpp::doRequest ( ipp_t* request, int fd, const char* filename ) const {
#if CUPS_17 > 0
    http_t *http = httpConnect2 ( m_host.toLatin1(), m_port, NULL, AF_UNSPEC, cupsEncryption(), 1, 1000, NULL );
#else
    http_t *http = httpConnect ( m_host.toLatin1(), m_port);
#endif
    char uri[1024];
    /* Use httpAssembleURIf for the printer-uri string */
    httpAssembleURI ( HTTP_URI_CODING_ALL, uri, sizeof ( uri ), m_scheme.toLatin1(), NULL, m_host.toLatin1(), m_port, m_resource.toLatin1() );

    ippAddString ( request, IPP_TAG_OPERATION, IPP_TAG_URI, "printer-uri", NULL, uri );
    ippAddString ( request, IPP_TAG_OPERATION, IPP_TAG_NAME, "requesting-user-name", NULL, cupsUser() );

    if ( fd >= 0 ) {
        return cupsDoIORequest ( http, request, m_resource.toLatin1(), -1, fd );
    } else if ( filename != NULL ) {
        return cupsDoFileRequest ( http, request, m_resource.toLatin1(), filename );
    } else {
        return cupsDoRequest ( http, request, m_resource.toLatin1() );
    }
}

QString QIpp::getExplanationForStateReason (QString reason) const {
	QString level = "Error";
	if (reason.endsWith("-report")) {
		reason.remove("-report");
	} else if (reason.endsWith("-warning")) {
		reason.remove("-warning");
	} else if (reason.endsWith("-error")) {
		reason.remove("-error");
	}

	return m_states->value(reason);
}

void QIpp::getPrinterStatus() {
#if CUPS_17 > 0
    ipp_t *request = ippNewRequest ( IPP_OP_GET_PRINTER_ATTRIBUTES );
#else
    ipp_t *request = ippNewRequest ( IPP_GET_PRINTER_ATTRIBUTES );
#endif
    ipp_t *response = doRequest ( request );

    ipp_attribute_t *attr;
    QString attrname;
    QString state = "", statereason = "";
    for ( attr = ippFirstAttribute ( response ); attr != NULL; attr = ippNextAttribute ( response ) ) {
        attrname = ippGetName ( attr );
        if ( attrname.startsWith ( "printer-state" ) && ippGetValueTag ( attr ) == IPP_TAG_ENUM ) {
            state = ippEnumString ( attrname.toLatin1(), ippGetInteger ( attr, 0 ) );
        } else if ( attrname.startsWith ( "printer-state-reason" ) && ippGetValueTag ( attr ) == IPP_TAG_KEYWORD ) {
            statereason = ippGetString ( attr, 0, NULL );
        }
    }
    if ( state != m_state || statereason != m_statereason ) {
        m_state = state;
        m_statereason = statereason;
        emit printerStatusChanged ( state, statereason );
    }
}

void QIpp::Print ( const char* filename ) const {
#if CUPS_17 > 0
    ipp_t *request = ippNewRequest ( IPP_OP_PRINT_JOB );
#else
    ipp_t *request = ippNewRequest ( IPP_PRINT_JOB );
#endif
    ippAddString ( request, IPP_TAG_OPERATION, IPP_TAG_NAME, "job-name", NULL, filename );
    ipp_t *response = doRequest ( request, -1, filename );
    handleJobResponse(response);
}

void QIpp::Print ( int fd, const char *jobname ) const {
#if CUPS_17 > 0
    ipp_t *request = ippNewRequest ( IPP_OP_PRINT_JOB );
#else
    ipp_t *request = ippNewRequest ( IPP_PRINT_JOB );
#endif
    ippAddString ( request, IPP_TAG_OPERATION, IPP_TAG_NAME, "job-name", NULL, jobname );
    ipp_t *response = doRequest ( request, fd );
    handleJobResponse(response);
}

void QIpp::handleJobResponse (ipp_t* response) const{
	ipp_attribute_t *attr;
	QString attrname;
	int jobid;
	QString jobstate = "", jobstatereason = "";
	QUrl joburi;
	for (attr = ippFirstAttribute (response); attr != NULL; attr = ippNextAttribute (response)) {
		attrname = ippGetName (attr);
		if (attrname.startsWith ("job-state") && ippGetValueTag (attr) == IPP_TAG_ENUM) {
			jobstate = ippEnumString (attrname.toLatin1(), ippGetInteger (attr, 0));
		} else if (attrname.startsWith ("job-state-reason") && ippGetValueTag (attr) == IPP_TAG_KEYWORD) {
			jobstatereason = ippGetString (attr, 0, NULL);
		} else if (attrname.startsWith("job-id") && ippGetValueTag(attr) == IPP_TAG_INTEGER) {
			jobid = ippGetInteger(attr, 0);
		} else if (attrname.startsWith("job-uri") && ippGetValueTag(attr) == IPP_TAG_URI) {
			joburi = QUrl(ippGetString(attr, 0, NULL));
		}
	}
}

void QIpp::setStatusInterval (int interval) {
	m_interval = interval;

	if (m_timer) {
		m_timer->setInterval (m_interval);
	}
}


#include "QIpp.moc"

