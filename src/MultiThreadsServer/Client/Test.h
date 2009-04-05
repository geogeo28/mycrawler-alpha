#ifndef TEST_H
#define TEST_H

#include <QtNetwork>

#include "Client.h"

class CTest : public QObject
{
  Q_OBJECT

public:
    CTest(QObject* parent = NULL);
    bool run();

public slots:
    void showError(QAbstractSocket::SocketError errorType);

private:
    CClient* m_pClient;
};

#endif // TEST_H
