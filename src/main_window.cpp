#include "main_window.h"
#include "ui_main_window.h"

#include <QMessageBox>
#include <QFileDialog>



MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_volume_data()
  , m_transfer_func()
{
  ui->setupUi(this);

  // signaly z volume vieweru
  connect(ui->volumeViewer, SIGNAL(error(const QString &)), SLOT(displayError(const QString &)));

  connect(ui->pbToggleRenderer, SIGNAL(clicked()), ui->volumeViewer, SLOT(toggleRenderer()));

  connect(ui->transferFuncEditor, SIGNAL(transferFunctionChanged(const TransferFunction *)),
          ui->volumeViewer, SLOT(setTransferFunction(const TransferFunction *)));

  connect(ui->pbLoadModel, SIGNAL(clicked()), this, SLOT(handleLoadModel()));
  connect(ui->pbDumpTF, SIGNAL(clicked()), this, SLOT(handleDumpTF()));
}


MainWindow::~MainWindow(void)
{
  delete ui;
}


void MainWindow::displayError(const QString & msg)
{
  QMessageBox::critical(this, tr("Error"), msg);
}


void MainWindow::handleLoadModel(void)
{
  QString filename(QFileDialog::getOpenFileName(this, tr("Load Model")));
  if (filename.isNull()) return;

#if 0
  if (!ui->volumeViewer->openFile(filename))
  {
    QMessageBox::critical(this, tr("Error"), tr("Failed open file %1").arg(filename));
    return;
  }
  else
  {
    qDebug() << "volume file loaded successfully";
  }

  ui->transferFuncEditor->updateHistogram(ui->volumeViewer->volumeData());
#endif

  setVolumeFile(filename);
}


void MainWindow::handleLoadTF(void)
{
  QString filename(QFileDialog::getOpenFileName(this, tr("Load Transfer Function")));
  if (filename.isNull()) return;

}


void MainWindow::handleSaveTF(void)
{
  QString filename(QFileDialog::getSaveFileName(this, tr("Save Transfer Function")));
  if (filename.isNull()) return;
}


void MainWindow::handleDumpTF(void)
{
  ui->transferFuncEditor->dumpTransferFunction();
}


void MainWindow::showEvent(QShowEvent *event)
{
#if 0
  // nacitanie defaultnych dat

  bool ret = ui->volumeViewer->openRawFile(":/data/head256x256x109_8bit_chan.raw", 256, 256, 109, 8);
  //ui->volumeViewer->setFile(":/data/head256x256x109_8bit_chan.raw");

  //bool ret = ui->volumeViewer->openFile("D:/AC601/VirtualBOXShare/Ubuntu/PGP/projekt/PGP_p4jos/data/8-bit/Foot.pvm");
  //bool ret = ui->volumeViewer->openFile("D:/AC601/VirtualBOXShare/Ubuntu/PGP/projekt/PGP_p4jos/data/16-bit/MRI-Woman.pvm");
  //bool ret = ui->volumeViewer->openFile("D:/AC601/VirtualBOXShare/Ubuntu/PGP/projekt/PGP_p4jos/data/16-bit/Teddy.pvm");

  qDebug() << "volume file loaded: " << (ret ? "yes" : "no");

  ui->transferFuncEditor->updateHistogram(ui->volumeViewer->volumeData());
#endif

  setRawVolumeFile(":/data/head256x256x109_8bit_chan.raw", 256, 256, 109, 8, 1.0f, 1.0f, 1.0f);

  return QMainWindow::showEvent(event);
}


void MainWindow::setVolumeFile(const QString & filename)
{
  if (!m_volume_data.load(filename))
  {
    displayError(tr("Failed to load model from file %1").arg(filename));
    return;
  }

  ui->volumeViewer->setVolumeData(&m_volume_data);
  ui->transferFuncEditor->updateHistogram(m_volume_data);
}


void MainWindow::setRawVolumeFile(const QString & filename, int width, int height, int depth, int bit_depth, float scalex, float scaley, float scalez)
{
  if (!m_volume_data.loadFromRaw(filename, width, height, depth, bit_depth))
  {
    displayError(tr("Failed to load model from raw file %1").arg(filename));
    return;
  }

  m_volume_data.setScale(scalex, scaley, scalez);

  ui->volumeViewer->setVolumeData(&m_volume_data);
  ui->transferFuncEditor->updateHistogram(m_volume_data);
}
