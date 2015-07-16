#include "mainwindow.h"
#include <QApplication>

#ifdef Q_OS_WIN
#include <QAxFactory>
#include "neuronfonts.h"

//                 IDTypeLib                                 IDApp
QAXFACTORY_BEGIN("{6B23B5A6-5D94-4E36-B972-562D11D479CC}", "{9A171A0A-087E-42C6-AA68-B1552F2A2249}")
   QAXCLASS(NeuronFonts)
QAXFACTORY_END()
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
