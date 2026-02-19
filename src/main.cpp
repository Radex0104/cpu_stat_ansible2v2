#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("CpuStatCheck");
    app.setOrganizationName("Radex");
    app.setApplicationVersion("1.5");

    MainWindow window;
    window.show();

    return app.exec();
}
