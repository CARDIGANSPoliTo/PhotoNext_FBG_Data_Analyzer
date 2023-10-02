#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mongointerface.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    MongoInterface *m_db;
};
#endif // MAINWINDOW_H
