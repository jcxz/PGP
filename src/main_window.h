#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "volume_data.h"
#include "transfer_function.h"

#include <QMainWindow>


namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow(void);

  private slots:
    void displayError(const QString & msg);
    void handleLoadModel(void);
    void handleLoadTF(void);
    void handleSaveTF(void);
    void handleDumpTF(void);
    void setTransferFunctionPreset(int preset_id);
    void handleRendererSwitch(void);
    void handleAutoSubsamplingChange(void);

  protected:
    virtual void showEvent(QShowEvent *event) override;

  private:
    void setVolumeFile(const QString & filename);
    void setRawVolumeFile(const QString & filename, int width, int height, int depth, int bit_depth, float scalex, float scaley, float scalez);

  private:
    Ui::MainWindow *ui;
    VolumeData m_volume_data;
    TransferFunction m_transfer_func;
    bool m_auto_subsampling_modified; // automatic subsampling changed by user
};

#endif // MAIN_WINDOW_H
