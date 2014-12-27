#include "main_window.h"
#include "ui_main_window.h"



MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // nacitanie defaultnych dat
  //ui->volumeViewer->openRawFile(":/data/head256x256x109_8bit_chan.raw", 256, 256, 109);

  ui->volumeViewer->setFile(":/data/head256x256x109_8bit_chan.raw");

  connect(ui->pbToggleRenderer, SIGNAL(clicked()), ui->volumeViewer, SLOT(toggleRenderer()));

  connect(ui->transferFuncEditor, SIGNAL(transferFunctionChanged(const TransferFunction *)),
          ui->volumeViewer, SLOT(setTransferFunction(const TransferFunction *)));
}


MainWindow::~MainWindow(void)
{
  delete ui;
}
