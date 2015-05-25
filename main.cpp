#include <QApplication>
#include <QtDebug>
#include <QObject>

#include <QFont>
#include "medeawindow.h"
#include "modeltester.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/Resources/Icons/medea.png"));
    a.setApplicationName("MEDEA");

    QCoreApplication::setOrganizationName("Defence Information Group");
    QCoreApplication::setApplicationName("MEDEA");

    QFont font = QFont("Verdana");
    font.setPointSizeF(8.5);


    a.setFont(font);

    QString GraphMLFile = 0;
    if (argc == 2) {
        GraphMLFile = QString(argv[1]);
    }

    MedeaWindow *w = new MedeaWindow(GraphMLFile);
    w->show();
    w->setupInitialSettings();

    return a.exec();
}
