#ifndef TRANSFER_FUNCTION_EDITOR_H
#define TRANSFER_FUNCTION_EDITOR_H

#include "volume_data_histogram.h"

#include <QWidget>


class VolumeData;
class TransferFunction;

class TransferFunctionEditor : public QWidget
{
    Q_OBJECT

  private:
    static constexpr int POINT_RENDER_RADIUS = 2;
    static constexpr int POINT_SELECT_RADIUS = 4;
    static constexpr int POINT_SEARCH_RADIUS = 8;
    static constexpr int AXIS_ARROW_SIZE     = 15;

    static constexpr int EXTRA_INNNER_PADDING_RIGHT = POINT_SELECT_RADIUS + 4;
    static constexpr int EXTRA_INNNER_PADDING_TOP   = POINT_SELECT_RADIUS + 4;

    static constexpr int INNER_PADDING_LEFT   = POINT_SELECT_RADIUS + AXIS_ARROW_SIZE;
    static constexpr int INNER_PADDING_RIGHT  = POINT_SELECT_RADIUS + AXIS_ARROW_SIZE + EXTRA_INNNER_PADDING_RIGHT;
    static constexpr int INNER_PADDING_TOP    = POINT_SELECT_RADIUS + AXIS_ARROW_SIZE + EXTRA_INNNER_PADDING_TOP;
    static constexpr int INNER_PADDING_BOTTOM = POINT_SELECT_RADIUS + AXIS_ARROW_SIZE;
    static constexpr int INNER_PADDING_X      = INNER_PADDING_LEFT + INNER_PADDING_RIGHT;
    static constexpr int INNER_PADDING_Y      = INNER_PADDING_TOP + INNER_PADDING_BOTTOM;

  public:
    explicit TransferFunctionEditor(QWidget *parent = 0);

  signals:
    void transferFunctionChanged(const TransferFunction *func);

  public slots:
    void setTransferFunction(TransferFunction *func);
    void updateHistogram(const VolumeData & data);

  protected:
    virtual void paintEvent(QPaintEvent * /* event */) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

  private:
    inline QPointF scaleToTCP(float x, float y)
    {
      return QPointF(x / float(width()  - INNER_PADDING_X),
                     y / float(height() - INNER_PADDING_Y));
    }

    inline QPointF toTCP(QPoint pt)
    {
      return QPointF(float(pt.x() - INNER_PADDING_LEFT) / float(width() - INNER_PADDING_X),
                     1.0f - (float(pt.y() - INNER_PADDING_TOP) / float(height() - INNER_PADDING_Y)));
    }

    inline QPoint fromTCP(QPointF pt)
    {
      return QPoint(pt.x() * (width() - INNER_PADDING_X) + INNER_PADDING_LEFT,
                    (1.0f - pt.y()) * (height() - INNER_PADDING_Y) + INNER_PADDING_TOP);
    }

  private:
    void drawGrid(QPainter & painter, int w, int h);

  private:
    int m_cur_tcp_idx;
    float m_volume_data_range;
    TransferFunction *m_transfer_func;
    VolumeDataHistogram m_volume_data_hist;
};

#endif // TRANSFER_FUNCTION_EDITOR_H
