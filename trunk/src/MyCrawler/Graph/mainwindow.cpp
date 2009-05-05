#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Debug/Exception.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);
}

MainWindow::~MainWindow()
{

}

void MainWindow::on_pushButton_clicked()
{
   Assert(false);
}
