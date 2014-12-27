#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>


namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow(void);

  private slots:
    void handleLoadModel(void);

  protected:
    virtual void showEvent(QShowEvent *event) override;

  private:
    Ui::MainWindow *ui;
};

#endif // MAIN_WINDOW_H
