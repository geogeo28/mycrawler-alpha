#include "Debug/Exception.h"

#include "UrlsInProgressTreeWidget.h"

static const MyQTreeWidgetHeaderItem ColumnsHeader[] = {
  (MyQTreeWidgetHeaderItem){NULL, "Hash Signature",  -1, true},
  (MyQTreeWidgetHeaderItem){NULL, "Depth",           -1, false},
  (MyQTreeWidgetHeaderItem){NULL, "Url",             -1, false}
};
static const int ColumnsHeaderCount = 3;
static const int ColumnSortedIndex = -1;

void MCUrlsInProgressTreeWidget::cleanAll_() {

}

MCUrlsInProgressTreeWidget::MCUrlsInProgressTreeWidget(QWidget* parent)
  : MyQTreeWidget(parent)
{}

MCUrlsInProgressTreeWidget::~MCUrlsInProgressTreeWidget() {
  cleanAll_();
}

void MCUrlsInProgressTreeWidget::setup() {
  setupHeader(ColumnsHeader, ColumnsHeaderCount);
  sortByColumn(ColumnSortedIndex, Qt::AscendingOrder);
  setPersistentColumnIndex(UrlColumn);
}
