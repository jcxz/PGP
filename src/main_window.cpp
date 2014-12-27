#include "main_window.h"
#include "ui_main_window.h"

#include <QMessageBox>
#include <QFileDialog>



MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  connect(ui->pbToggleRenderer, SIGNAL(clicked()), ui->volumeViewer, SLOT(toggleRenderer()));

  connect(ui->transferFuncEditor, SIGNAL(transferFunctionChanged(const TransferFunction & )),
          ui->volumeViewer, SLOT(setTransferFunction(const TransferFunction & )));

  connect(ui->pbLoadModel, SIGNAL(clicked()), this, SLOT(handleLoadModel()));
}


MainWindow::~MainWindow(void)
{
  delete ui;
}


void MainWindow::handleLoadModel(void)
{
  QString filename(QFileDialog::getOpenFileName(this, tr("Load Model")));
  if (filename.isNull()) return;

  if (!ui->volumeViewer->openFile(filename))
  {
    QMessageBox::critical(this, tr("Error"), tr("Failed open file %1").arg(filename));
  }
  else
  {
    qDebug() << "volume file loaded successfully";
  }
}


void MainWindow::showEvent(QShowEvent *event)
{
  // nacitanie defaultnych dat
  //ui->volumeViewer->openRawFile(":/data/head256x256x109_8bit_chan.raw", 256, 256, 109);

  //ui->volumeViewer->setFile(":/data/head256x256x109_8bit_chan.raw");
  //ui->volumeViewer->setFile("D:/AC601/VirtualBOXShare/Ubuntu/PGP/projekt/PGP_p4jos/data/8-bit/Foot.pvm");
  //bool ret = ui->volumeViewer->openFile("D:/AC601/VirtualBOXShare/Ubuntu/PGP/projekt/PGP_p4jos/data/8-bit/Foot.pvm");
  bool ret = ui->volumeViewer->openFile("D:/AC601/VirtualBOXShare/Ubuntu/PGP/projekt/PGP_p4jos/data/16-bit/MRI-Woman.pvm");

  qDebug() << "volume file loaded: " << (ret ? "yes" : "no");

  return QMainWindow::showEvent(event);
}
