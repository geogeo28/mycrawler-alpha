#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass)
{
    ui->setupUi(this);
}

/**
 * @author Julien
 * @pre i==0
 */
MainWindow::~MainWindow()
{
    delete ui;
}
