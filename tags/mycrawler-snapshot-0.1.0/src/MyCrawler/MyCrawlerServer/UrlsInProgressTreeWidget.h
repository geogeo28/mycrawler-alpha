#ifndef URLSINPROGRESSTREEWIDGET_H
#define URLSINPROGRESSTREEWIDGET_H

#include "Widgets/MyQTreeWidget.h"

#include "UrlInfo.h"

#include "ClientThread.h"

class MCUrlsInProgressTreeWidget : public MyQTreeWidget
{
    Q_OBJECT

public:
    enum {
      ThreadIDColumn,
      HostNameColumn,
      PeerAddressColumn,
      PeerPortColumn,
      HashSignatureColumn,
      DepthColumn,
      UrlColumn,
    };

private:
    void cleanAll_();

public:
    MCUrlsInProgressTreeWidget(QWidget* parent = NULL);
    ~MCUrlsInProgressTreeWidget();

    void setup();

private slots:
    void slotAddUrl_(MCClientThread* client, MCUrlInfo urlInfo);
    void slotRemoveUrl_(MCClientThread* client, MCUrlInfo urlInfo);
};

#endif // URLSINPROGRESSTREEWIDGET_H
