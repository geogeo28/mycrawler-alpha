#ifndef MCGRAPHMAINWINDOW_H
#define MCGRAPHMAINWINDOW_H
#include "ui_GraphMainWindow.h"
#include <QtGui/QMainWindow>

class MCGraphMainWindow : public QMainWindow,
                        public Ui_MCGraphMainWindow
{
    Q_OBJECT

public:
    MCGraphMainWindow(QWidget *parent = 0);
    ~MCGraphMainWindow();

public slots:
    void on_pushButton_clicked();
};

#endif // MCGRAPHMAINWINDOW_H
