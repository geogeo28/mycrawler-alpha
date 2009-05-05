#ifndef MCGRAPHAPPLICATION_H
#define MCGRAPHAPPLICATION_H
#include "mainwindow.h"
#include <QtGui/QMainWindow>
#include <QPointer>
class MCGraphApplication : public IApplication
{
public:
    MainWindow GraphMainWindow;
    static MCGraphApplication* instance();
    MCGraphApplication(int &argc, char** argv);
    void run();

private :
    static MCGraphApplication* s_instance;
};

#endif // MCGRAPHAPPLICATION_H
