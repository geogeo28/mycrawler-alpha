#include <QtCore/QCoreApplication>

#include "Test.h"

int main(int argc, char *argv[])
{
  CTest test;

  QCoreApplication a(argc, argv);

  qDebug() << "Example of Multi-threads server (client)" << endl
           << "Copyright (C) 2009 by ANNEHEIM Geoffrey" << endl;

  test.run();

  return a.exec();
}
