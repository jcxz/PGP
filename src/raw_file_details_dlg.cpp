#include "raw_file_details_dlg.h"
#include "ui_raw_file_details_dlg.h"



RawFileDetailsDlg::RawFileDetailsDlg(QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::RawFileDetailsDlg)
{
  ui->setupUi(this);

  // Nastavenie zoznamu podporovanych bitovych hlbok
  ui->cbDataType->addItem(tr("8-bit Unsigned Int"), QVariant(8));
  ui->cbDataType->addItem(tr("16-bit Unsigned Int"), QVariant(16));

  // zakazanie zvacsovania dialogu
  layout()->setSizeConstraint(QLayout::SetFixedSize);
}

RawFileDetailsDlg::~RawFileDetailsDlg(void)
{
  delete ui;
}

int RawFileDetailsDlg::dataWidth(void) const { return ui->sbWidth->value(); }
int RawFileDetailsDlg::dataHeight(void) const { return ui->sbHeight->value(); }
int RawFileDetailsDlg::dataDepth(void) const { return ui->sbDepth->value(); }
int RawFileDetailsDlg::dataBitDepth(void) const { return ui->cbDataType->currentData().toInt(); }
float RawFileDetailsDlg::dataScaleX(void) const { return ui->sbScaleX->value(); }
float RawFileDetailsDlg::dataScaleY(void) const { return ui->sbScaleY->value(); }
float RawFileDetailsDlg::dataScaleZ(void) const { return ui->sbScaleZ->value(); }
