#include "Test.h"
#include <cstdio>
#include <iostream>

using namespace std;

CTest::CTest(QObject* parent)
    : QObject(parent)
{
  m_pClient = new CClient(this);

  QObject::connect(
      m_pClient, SIGNAL(error(QAbstractSocket::SocketError)),
      this, SLOT(showError(QAbstractSocket::SocketError))
  );
}

bool CTest::run() {
  string sAddress;
  int nPort;

  cout << "Host address : ";
  cin >> sAddress;

  cout << "Host port : ";
  cin >> nPort;

  cout << "Connecting to server..." << endl;
  m_pClient->connectToHost(QHostAddress(sAddress.data()), nPort);

  return true;
}

void CTest::showError(QAbstractSocket::SocketError errorType) {
  qDebug() << QString("Error (%1)").arg(errorType);
}
