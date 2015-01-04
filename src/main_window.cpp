#include "main_window.h"
#include "ui_main_window.h"
#include "raw_file_details_dlg.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>



MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_volume_data()
  , m_transfer_func()
  , m_auto_subsampling_modified(false)
{
  ui->setupUi(this);

  // hide debugging widgets
  ui->pbDumpTF->hide();
  ui->pbToggleRenderer->hide();

  // connect to an error handler
  connect(ui->volumeViewer, SIGNAL(error(const QString &)), SLOT(displayError(const QString &)));

  // render in high quality after the transfer function has been changed
  connect(ui->transferFuncEditor, SIGNAL(transferFunctionChanged(const TransferFunction *)),
          ui->volumeViewer, SLOT(setHighQuality()));
  connect(ui->transferFuncEditor, SIGNAL(transferFunctionChanged(const TransferFunction *)),
          ui->volumeViewer, SLOT(setTransferFunction(const TransferFunction *)));

  // render in preview quality while the transfer function is being manipulated
  connect(ui->transferFuncEditor, SIGNAL(transferFunctionManipulated(const TransferFunction*)),
          ui->volumeViewer, SLOT(setLowQuality()));
  connect(ui->transferFuncEditor, SIGNAL(transferFunctionManipulated(const TransferFunction *)),
          ui->volumeViewer, SLOT(setTransferFunction(const TransferFunction*)));

  // connect sliders for changing the detail (again render in low quality during manipulation)
  connect(ui->sliderOptionsCustomDetailLevel, SIGNAL(sliderPressed()), ui->volumeViewer, SLOT(setLowQuality()));
  connect(ui->sliderOptionsCustomDetailLevel, SIGNAL(valueChanged(int)), ui->volumeViewer, SLOT(setDetail(int)));
  connect(ui->sliderOptionsCustomDetailLevel, SIGNAL(sliderReleased()), ui->volumeViewer, SLOT(setHighQuality()));

  // create a list of transfer function presets and sample models
  ui->cbTransferFunctionPresets->addItem(tr("Woman"), 0);
  ui->cbTransferFunctionPresets->addItem(tr("Male"), 1);
  connect(ui->cbTransferFunctionPresets, SIGNAL(currentIndexChanged(int)), SLOT(handleTFPresetSwitch()));

  // QPoint tu pouzivam ako QPair, prve cislo je id modelu a druhe je id transfer funkcie
  ui->cbSampleModels->addItem(tr("Woman"), QPoint(0, 0));
  ui->cbSampleModels->addItem(tr("Male"), QPoint(1, 1));
  connect(ui->cbSampleModels, SIGNAL(currentIndexChanged(int)), SLOT(handleSampleModelSwitch()));
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

  QString ext = QFileInfo(filename).suffix();

  bool do_open_raw = false;

  if (ext == "raw")
  {
    do_open_raw = true;
  }
  else if (!VolumeData::supportedFileFormatExtensions().contains(ext))
  {
    if (QMessageBox::question(this,
                              tr("Unknown file extension"),
                              tr("This file extension is not known, do you still want to open the file as raw ?"),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
    {
      return;
    }
    do_open_raw = true;
  }

  if (do_open_raw)
  {
    RawFileDetailsDlg dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
      setRawVolumeFile(filename,
                       dlg.dataWidth(), dlg.dataHeight(), dlg.dataDepth(),
                       dlg.dataBitDepth(),
                       dlg.dataScaleX(), dlg.dataScaleY(), dlg.dataScaleZ());
    }
  }
  else
  {
    setVolumeFile(filename);
  }
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
    case 0:  // Transfer Function preset for 'woman' data set
      m_transfer_func.clear();
      m_transfer_func.addTCP(QPointF(0.05859375f, 0.00f), QColor(Qt::red));
      m_transfer_func.addTCP(QPointF(0.19531250f, 0.05f), QColor(Qt::green));
      m_transfer_func.addTCP(QPointF(0.39062500f, 0.03f), QColor(Qt::blue));
      m_transfer_func.addTCP(QPointF(0.58593750f, 0.10f), QColor(Qt::yellow));
      m_transfer_func.addTCP(QPointF(0.78125000f, 0.06f), QColor(Qt::magenta));
      m_transfer_func.addTCP(QPointF(1.00000000f, 0.50f), QColor(Qt::cyan));
      break;

    case 1:  // Transfer Function preset for 'Male' data set
      m_transfer_func.clear();
      m_transfer_func.addTCP(QPointF(0.00000000f, 0.00f), QColor::fromRgbF(0.91f, 0.70f, 0.61f));
      m_transfer_func.addTCP(QPointF(0.15625000f, 0.00f), QColor::fromRgbF(0.91f, 0.70f, 0.61f));
      m_transfer_func.addTCP(QPointF(0.23437500f, 0.20f), QColor::fromRgbF(0.91f, 0.70f, 0.61f));
      m_transfer_func.addTCP(QPointF(0.24609375f, 0.05f), QColor::fromRgbF(0.91f, 0.70f, 0.61f));
      m_transfer_func.addTCP(QPointF(0.31250000f, 0.00f), QColor::fromRgbF(0.91f, 0.70f, 0.61f));
      m_transfer_func.addTCP(QPointF(0.32031250f, 0.90f), QColor::fromRgbF(1.00f, 1.00f, 0.85f));
      m_transfer_func.addTCP(QPointF(1.00000000f, 1.00f), QColor::fromRgbF(1.00f, 1.00f, 0.85f));
      break;
  }

  ui->transferFuncEditor->setTransferFunction(&m_transfer_func);
}


void MainWindow::setSampleModel(int model_id)
{
  switch (model_id)
  {
    case 0:
      setRawVolumeFile(":/data/head_256x256x109_8bit_1-0x1-0x1-5.raw",
                       256, 256, 109, 8, 1.0f, 1.0f, 1.0f);
      break;

    case 1:
      setRawVolumeFile(":/data/VisMale_128x256x256_8bit_1-57774x0-995861x1-00797.raw",
                       128, 256, 256, 8, 1.57774f, 0.995861f, 1.00797f);
      break;
  }
}


void MainWindow::handleRendererSwitch(void)
{
  if (ui->rbOptionsRayCastingRenderer->isChecked())
  {
    ui->volumeViewer->setRenderer(VolumeViewer::RayCastRenderer);
    if (!m_auto_subsampling_modified)
    {
      ui->volumeViewer->setAutoSubsampling(false);
      ui->cbOptionsAutoSubsampling->setChecked(false);
    }
  }
  else if (ui->rbOptionsTextureRenderer->isChecked())
  {
    ui->volumeViewer->setRenderer(VolumeViewer::TextureRenderer);
    if (!m_auto_subsampling_modified)
    {
      ui->volumeViewer->setAutoSubsampling(true);
      ui->cbOptionsAutoSubsampling->setChecked(true);
    }
  }
  else
  {
    qWarning() << "Unknown renderer";
  }
}


void MainWindow::handleAutoSubsamplingChange(void)
{
  m_auto_subsampling_modified = true;
  ui->volumeViewer->setAutoSubsampling(ui->cbOptionsAutoSubsampling->isChecked());
}


void MainWindow::handleDetailChange(void)
{
  if (ui->gbOptionsCustomDetail->isChecked())
  {
    ui->volumeViewer->setDetail(ui->sliderOptionsCustomDetailLevel->value());
  }
  else
  {
    ui->volumeViewer->setDetail(0);
  }
}


void MainWindow::handleTFPresetSwitch(void)
{
  int id = ui->cbTransferFunctionPresets->currentData().toInt();
  setTransferFunctionPreset(id);
}


void MainWindow::handleSampleModelSwitch(void)
{
  QPoint pair = ui->cbSampleModels->currentData().toPoint();
  setSampleModel(pair.x());
  ui->cbTransferFunctionPresets->setCurrentIndex(pair.y());
}


void MainWindow::showEvent(QShowEvent *event)
{
  // nastavenie defaultnych dat a transfer funkcie
  ui->cbSampleModels->setCurrentIndex(1);   // ak by tu bola 0, tak by to nefungovalo, pretoze 0 je uz na zaciatku current index
  //setSampleModel(1);
  //setTransferFunctionPreset(1);

  // nastavenie defaultnych nastaveni zobrazovania
  //ui->volumeViewer->setAutoSubsampling(ui->cbOptionsAutoSubsampling->isChecked());
  //ui->volumeViewer->setDisplayBBox(ui->cbOptionsDisplayBBox->isChecked());
  ui->volumeViewer->setUseTransferFunction(ui->gbTransferFunctionEditor->isChecked());

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
