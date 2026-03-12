#include "mainwindow.h"
#include <QApplication>
#include "progressmanager.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("RPLM installer");
    app.setOrganizationName("CT-42 Skuratov Daniil");
    app.setApplicationVersion("1.5");

    MainWindow window;
    window.show();

    return app.exec();
}
