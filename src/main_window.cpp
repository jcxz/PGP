#include "main_window.h"
#include "ui_main_window.h"



MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  connect(ui->pbToggleRenderer, SIGNAL(clicked()), ui->volumeViewer, SLOT(toggleRenderer()));
}


MainWindow::~MainWindow(void)
{
  delete ui;
}
