#include "raw_file_details_dlg.h"
#include "ui_raw_file_details_dlg.h"



RawFileDetailsDlg::RawFileDetailsDlg(QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::RawFileDetailsDlg)
{
  ui->setupUi(this);
}

RawFileDetailsDlg::~RawFileDetailsDlg(void)
{
  delete ui;
}

int RawFileDetailsDlg::dataWidth(void) const { return ui->sbWidth->value(); }
int RawFileDetailsDlg::dataHeight(void) const { return ui->sbHeight->value(); }
int RawFileDetailsDlg::dataDepth(void) const { return ui->sbDepth->value(); }
int RawFileDetailsDlg::dataBitDepth(void) const { return ui->sbBitDepth->value(); }
float RawFileDetailsDlg::dataScaleX(void) const { return ui->sbScaleX->value(); }
float RawFileDetailsDlg::dataScaleY(void) const { return ui->sbScaleY->value(); }
float RawFileDetailsDlg::dataScaleZ(void) const { return ui->sbScaleZ->value(); }
