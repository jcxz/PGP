#ifndef TRANSFER_FUNCTION_EDITOR_H
#define TRANSFER_FUNCTION_EDITOR_H

#include "transfer_function.h"

#include <QWidget>


class TransferFunctionEditor : public QWidget
{
    Q_OBJECT

  public:
    explicit TransferFunctionEditor(QWidget *parent = 0)
      : QWidget(parent)
      , m_transfer_func()
      , m_cur_pt(nullptr)
    {
      setFocusPolicy(Qt::ClickFocus);
      setFixedHeight(100);

      m_transfer_func.addControlPoint(toTCP2(QPoint(15,   0), 256, 100), QColor(Qt::red));
      m_transfer_func.addControlPoint(toTCP2(QPoint(50,   5), 256, 100), QColor(Qt::green));
      m_transfer_func.addControlPoint(toTCP2(QPoint(100,  3), 256, 100), QColor(Qt::blue));
      m_transfer_func.addControlPoint(toTCP2(QPoint(150, 10), 256, 100), QColor(Qt::yellow));
      m_transfer_func.addControlPoint(toTCP2(QPoint(200,  6), 256, 100), QColor(Qt::magenta));
      m_transfer_func.addControlPoint(toTCP2(QPoint(256, 50), 256, 100), QColor(Qt::cyan));
    }

  signals:
    void transferFunctionChanged(const TransferFunction & func);

  public slots:

  protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;

    virtual void showEvent(QShowEvent *event) override;

  private:
    inline QPointF toTCP(QPoint pt)
    {
      return QPointF(float(pt.x()) / float(width()), float(pt.y()) / float(height()));
    }

    inline QPointF toTCP2(QPoint pt, int w, int h)
    {
      return QPointF(float(pt.x()) / float(w), float(pt.y()) / float(h));
    }

    inline QPoint fromTCP(QPointF pt)
    {
      return QPoint(pt.x() * width(), pt.y() * height());
    }

  private:
    TransferFunction m_transfer_func;
    TransferControlPoint *m_cur_pt;
};

#endif // TRANSFER_FUNCTION_EDITOR_H
