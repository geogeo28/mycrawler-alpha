#include "GraphMainWindow.h"
#include "Debug/Exception.h"
#include "Debug/Logger.h"
#include "Graph.h"
#include <iostream>

MCGraphMainWindow::MCGraphMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);
}

MCGraphMainWindow::~MCGraphMainWindow()
{

}

void MCGraphMainWindow::on_pushButton_clicked()
{
    MCGraph graph;
    graph.display();

}
