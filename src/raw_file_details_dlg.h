#ifndef RAW_FILE_DETAILS_DLG_H
#define RAW_FILE_DETAILS_DLG_H

#include <QDialog>


namespace Ui { class RawFileDetailsDlg; }

class RawFileDetailsDlg : public QDialog
{
    Q_OBJECT

  public:
    explicit RawFileDetailsDlg(QWidget *parent = 0);
    ~RawFileDetailsDlg(void);

    int dataWidth(void) const;
    int dataHeight(void) const;
    int dataDepth(void) const;
    int dataBitDepth(void) const;
    float dataScaleX(void) const;
    float dataScaleY(void) const;
    float dataScaleZ(void) const;

  private:
    Ui::RawFileDetailsDlg *ui;
};

#endif // RAW_FILE_DETAILS_DLG_H
