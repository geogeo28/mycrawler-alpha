#include <QtGui/QApplication>
#include <QWidget>
#include "mainwindow.h"
#include "mcgraphapplication.h"

int main(int argc, char *argv[])
{
    try{
        MCGraphApplication app(argc, argv);
        app.run();
        return app.exec();
    }
    catch(const CException& e){
        e.dialog();
        throw;
    }
}
