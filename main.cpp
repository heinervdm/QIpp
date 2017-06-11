#include <QCoreApplication>
#include "QIpp.h"
#include <QUrl>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QIpp qipp(new QUrl("ipp://10.16.0.140:631/printer/ipp"));
    return app.exec();
}
