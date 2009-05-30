#ifndef URLSINPROGRESSTREEWIDGET_H
#define URLSINPROGRESSTREEWIDGET_H

#include "Widgets/MyQTreeWidget.h"

class MCUrlsInProgressTreeWidget : public MyQTreeWidget
{
    Q_OBJECT

public:
    enum {
      HashSignatureColumn,
      DepthColumn,
      UrlColumn
    };

private:
    void cleanAll_();

public:
    MCUrlsInProgressTreeWidget(QWidget* parent = NULL);
    ~MCUrlsInProgressTreeWidget();

    void setup();
};

#endif // URLSINPROGRESSTREEWIDGET_H
