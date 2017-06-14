#include <QApplication>
#include "StatusGui.h"
#include <QUrl>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
// 	QIpp qipp(new QUrl("ipp://10.132.139.33:631/ipp"));
	StatusGui stgui;
	stgui.show();
	return app.exec();
}
