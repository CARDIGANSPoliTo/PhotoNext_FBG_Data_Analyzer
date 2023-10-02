#include "mainwindow.h"
#include "customstatusbar.h"
#include "qtabbar.h"
#include "sessionsettings.h"
#include "ui_mainwindow.h"
#include "custommenubar.h"
#include "peaktimeview.h"
#include "measuresview.h"
#include "fftview.h"
#include "qmenubar.h"

MongoInterface g_db{};
CustomStatusBar *g_statusBar;
SessionSettings *g_settings;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    g_settings = new SessionSettings();

    QTabWidget *tabView = new QTabWidget(this);
    tabView->addTab(new PeakTimeView(tabView), "Main");
    tabView->addTab(new MeasuresView(tabView), "Measures");
    tabView->addTab(new FFTView(tabView), "FFT");
    tabView->setTabPosition(QTabWidget::East);
    tabView->setDocumentMode(true);
    tabView->tabBar()->setStyleSheet("QTabBar::tab {font-size:12pt; width: 32px;}");

    g_statusBar = new CustomStatusBar();

    setCentralWidget(tabView);
    setMenuBar(new CustomMenuBar());
    setStatusBar(g_statusBar);
    setWindowTitle("PolitoSmartSoft");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete g_statusBar;
    delete g_settings;
}

