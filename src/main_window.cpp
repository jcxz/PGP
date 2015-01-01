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

  ui->pbDumpTF->hide();
  ui->pbToggleRenderer->hide();

  connect(ui->volumeViewer, SIGNAL(error(const QString &)), SLOT(displayError(const QString &)));

  connect(ui->pbToggleRenderer, SIGNAL(clicked()), ui->volumeViewer, SLOT(toggleRenderer()));

  connect(ui->transferFuncEditor, SIGNAL(transferFunctionChanged(const TransferFunction *)),
          ui->volumeViewer, SLOT(setTransferFunction(const TransferFunction *)));

  connect(ui->pbLoadModel, SIGNAL(clicked()), this, SLOT(handleLoadModel()));
  connect(ui->pbLoadTF, SIGNAL(clicked()), this, SLOT(handleLoadTF()));
  connect(ui->pbSaveTF, SIGNAL(clicked()), this, SLOT(handleSaveTF()));
  connect(ui->pbDumpTF, SIGNAL(clicked()), this, SLOT(handleDumpTF()));

  connect(ui->sliderOptionsCustomDetailLevel, SIGNAL(valueChanged(int)), ui->volumeViewer, SLOT(setDetail(int)));
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

  if (!m_transfer_func.load(filename))
  {
    displayError(tr("Failed to load transfer function from file %1").arg(filename));
    return;
  }

  ui->transferFuncEditor->setTransferFunction(&m_transfer_func);
}


void MainWindow::handleSaveTF(void)
{
  QString filename(QFileDialog::getSaveFileName(this, tr("Save Transfer Function")));
  if (filename.isNull()) return;

  if (!m_transfer_func.save(filename))
  {
    displayError(tr("Failed to save transfer function to file %1").arg(filename));
  }
}


void MainWindow::handleDumpTF(void)
{
  qDebug() << m_transfer_func;
}


void MainWindow::setTransferFunctionPreset(int preset_id)
{
  switch (preset_id)
  {
    case 1:
      m_transfer_func.clear();
      m_transfer_func.addTCP(QPointF(0.05859375f, 0.00f), QColor(Qt::red));
      m_transfer_func.addTCP(QPointF(0.19531250f, 0.05f), QColor(Qt::green));
      m_transfer_func.addTCP(QPointF(0.39062500f, 0.03f), QColor(Qt::blue));
      m_transfer_func.addTCP(QPointF(0.58593750f, 0.10f), QColor(Qt::yellow));
      m_transfer_func.addTCP(QPointF(0.78125000f, 0.06f), QColor(Qt::magenta));
      m_transfer_func.addTCP(QPointF(1.00000000f, 0.50f), QColor(Qt::cyan));
      break;
  }

  ui->transferFuncEditor->setTransferFunction(&m_transfer_func);
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
  setTransferFunctionPreset(1);

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
