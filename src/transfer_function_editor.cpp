#include "transfer_function_editor.h"
#include "transfer_function.h"
#include "volume_data.h"

#include <QColorDialog>
#include <QMouseEvent>
#include <QPainter>
#include <QBrush>
#include <QDebug>
#include <QStaticText>
#include <cstdint>



namespace {

void drawPoint(QPainter & painter, QPoint pt, int r, const QColor & c)
{
  QPen pen(c);
  pen.setWidth(5);

  QBrush brush(c);

  painter.setPen(pen);
  painter.setBrush(brush);

  painter.drawEllipse(pt, r, r);
}

}


TransferFunctionEditor::TransferFunctionEditor(QWidget *parent)
  : QWidget(parent)
  , m_cur_tcp_idx(TransferFunction::INVALID_TCP_INDEX)
  , m_volume_data_range(255.0f)
  , m_transfer_func(nullptr)
  , m_volume_data_hist()
{
  setFocusPolicy(Qt::ClickFocus);
  setMinimumSize(QSize(INNER_PADDING_X + 30, INNER_PADDING_Y + 30));
}


#if 0
void TransferFunctionEditor::initTest(void)
{
  setFixedHeight(100);
  if (m_transfer_func != nullptr)
  {
    m_transfer_func->addTCP(toTCP2(QPoint(15,   0), 256, 100), QColor(Qt::red));
    m_transfer_func->addTCP(toTCP2(QPoint(50,   5), 256, 100), QColor(Qt::green));
    m_transfer_func->addTCP(toTCP2(QPoint(100,  3), 256, 100), QColor(Qt::blue));
    m_transfer_func->addTCP(toTCP2(QPoint(150, 10), 256, 100), QColor(Qt::yellow));
    m_transfer_func->addTCP(toTCP2(QPoint(200,  6), 256, 100), QColor(Qt::magenta));
    m_transfer_func->addTCP(toTCP2(QPoint(256, 50), 256, 100), QColor(Qt::cyan));
  }
}
#endif


void TransferFunctionEditor::setTransferFunction(TransferFunction *func)
{
  m_transfer_func = func;
  emit transferFunctionChanged(m_transfer_func);
}


void TransferFunctionEditor::updateHistogram(const VolumeData & data)
{
  if (data.bitDepth() == 8)
  {
    m_volume_data_hist.rebuild((uint8_t *) data.data(), data.voxelCount());
  }
  else if (data.bitDepth() == 16)
  {
    m_volume_data_hist.rebuild((uint16_t *) data.data(), data.voxelCount());
  }

  m_volume_data_range = data.maxIntensity();
}


void TransferFunctionEditor::drawGrid(QPainter & painter, int w, int h)
{
  QColor c(128, 128, 128, 128);

  QPen pen(c);
  pen.setWidth(1);

  QBrush brush(c);

  painter.setPen(pen);
  painter.setBrush(brush);

  // vykreslenie samotnej mriezky
  painter.setRenderHint(QPainter::Antialiasing, false);
  painter.setRenderHint(QPainter::HighQualityAntialiasing, false);

  const int vert_step = 20;
  const int horz_step = 10;

  // horizontalne ciary mriezky
  for (int i = INNER_PADDING_BOTTOM; i < h - INNER_PADDING_TOP; i += horz_step)
  {
    painter.drawLine(INNER_PADDING_LEFT, h - i, w - INNER_PADDING_RIGHT, h - i);
  }

  // vertikalne ciary mriezky
  for (int i = INNER_PADDING_LEFT; i < w - INNER_PADDING_RIGHT; i += vert_step)
  {
    painter.drawLine(i, INNER_PADDING_TOP, i, h - INNER_PADDING_BOTTOM);
  }

  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

  pen.setWidth(2);
  painter.setPen(pen);

  // vykreslenie horizontalnej osi
  const QPointF triangle_horz[3] = {
    QPointF(w - INNER_PADDING_RIGHT + EXTRA_INNNER_PADDING_RIGHT,                   h - INNER_PADDING_BOTTOM - 2.5f),
    QPointF(w - INNER_PADDING_RIGHT + EXTRA_INNNER_PADDING_RIGHT + AXIS_ARROW_SIZE, h - INNER_PADDING_BOTTOM + 0.0f),
    QPointF(w - INNER_PADDING_RIGHT + EXTRA_INNNER_PADDING_RIGHT,                   h - INNER_PADDING_BOTTOM + 2.5f)
  };

  painter.drawLine(INNER_PADDING_LEFT, h - INNER_PADDING_BOTTOM, w - INNER_PADDING_RIGHT + EXTRA_INNNER_PADDING_RIGHT, h - INNER_PADDING_BOTTOM);
  painter.drawPolygon(triangle_horz, 3);

  // vykreslenie vertikalnej osi
  const QPointF triangle_vert[3] = {
    //                       x      y
    QPointF(INNER_PADDING_LEFT - 2.5f, INNER_PADDING_TOP - EXTRA_INNNER_PADDING_TOP), // lavy bod
    QPointF(INNER_PADDING_LEFT + 2.5f, INNER_PADDING_TOP - EXTRA_INNNER_PADDING_TOP), // pravy bod
    QPointF(INNER_PADDING_LEFT + 0.0f, INNER_PADDING_TOP - EXTRA_INNNER_PADDING_TOP - AXIS_ARROW_SIZE)  // stredny bod
  };

  painter.drawLine(INNER_PADDING_LEFT, h - INNER_PADDING_BOTTOM, INNER_PADDING_LEFT, INNER_PADDING_TOP - EXTRA_INNNER_PADDING_TOP);
  painter.drawPolygon(triangle_vert, 3);

  // vykreslenie horizontalneho popisku
  painter.drawStaticText(w - INNER_PADDING_RIGHT - 40, h - INNER_PADDING_BOTTOM + 2, QStaticText("intensity"));

  // vykreslenie vertikalneho popisku
  {
    QTransform old_t = painter.transform();
    QTransform t;
    t.translate(0, INNER_PADDING_TOP + 30);
    t.rotate(-90);
    painter.setTransform(t);
    painter.drawStaticText(0, 0, QStaticText("opacity"));
    painter.setTransform(old_t);
  }
}


void TransferFunctionEditor::paintEvent(QPaintEvent * /* event */)
{
  QPainter painter;

  painter.begin(this);

  // zapnutie antialiasingu
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::HighQualityAntialiasing);

  //QPen old_pen = painter.pen();
  QBrush old_brush = painter.brush();

  QPen pen;
  QBrush brush;

  // vykreslenie pozadia
  pen.setColor(QColor(Qt::white));
  brush.setColor(QColor(255, 255, 255, 128));
  brush.setStyle(Qt::SolidPattern);
  painter.setPen(pen);
  painter.setBrush(brush);

  painter.drawRect(rect());

  // vykreslenie mriezky a sipok
  drawGrid(painter, width(), height());

  // vykreslenie histogramu dat
  if (!m_volume_data_hist.isEmpty())
  {
    pen.setColor(QColor(128, 0, 0, 128));
    brush.setColor(QColor(128, 0, 0, 128));

    painter.setPen(pen);
    painter.setBrush(brush);

    float max = m_volume_data_hist.max();

    //qDebug() << "Plotting histogram";
    //hist.dump(qDebug(), m_volume_data->bitDepth());

    for (int i = INNER_PADDING_LEFT; i < (width() - INNER_PADDING_RIGHT); ++i)
    {
      int hist_val = m_volume_data_hist.value((float(i - INNER_PADDING_LEFT) / float(width() - INNER_PADDING_X)) * m_volume_data_range);
      int y = (float(hist_val) / float(max)) * (height() - INNER_PADDING_Y) + INNER_PADDING_BOTTOM;
      //int y = (float(y) / float(range)) * (height() - INNER_PADDING_Y) + INNER_PADDING_BOTTOM;

      //y *= 0.7f;

      painter.drawLine(QPoint(i, height() - INNER_PADDING_BOTTOM), QPoint(i, height() - y));
    }
  }
  else
  {
    qWarning() << "Not drawing volume data histogram because it is empty";
  }

  // nakreslenie krivky medzi kontrolnymi bodmi transfer funkcie
  if (m_transfer_func)
  {
    pen.setColor(QColor(Qt::red));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(old_brush);

    const TransferFunction::tContainer & cpts = m_transfer_func->controlPoints();

    for (int i = 1; i < cpts.size(); ++i)
    {
      painter.drawLine(fromTCP(cpts[i - 1].position()),
          fromTCP(cpts[i + 0].position()));
    }

    // vykreslenie kontrolnych bodov transfer funkcie
    for (int i = 0; i < cpts.size(); ++i)
    {
      const TransferControlPoint & p = cpts[i];

      // nakresli podsvietenie pre vybrany kontrolny bod
      if (i == m_cur_tcp_idx)
      {
        QColor c(128, 128, 128, 128);
        drawPoint(painter, fromTCP(p.position()), POINT_SELECT_RADIUS, c); //QColor(Qt::black));
      }

      drawPoint(painter, fromTCP(p.position()), POINT_RENDER_RADIUS, p.color());
    }
  }
  else
  {
    qWarning() << "Not drawing transfer function because m_transfer_func is NULL";
  }

  painter.end();
}


void TransferFunctionEditor::mousePressEvent(QMouseEvent *event)
{
  m_cur_tcp_idx = m_transfer_func->findByPosition(toTCP(event->pos()),
                                                  scaleToTCP(POINT_SEARCH_RADIUS,
                                                             POINT_SEARCH_RADIUS));

  update();

  return QWidget::mousePressEvent(event);
}


void TransferFunctionEditor::mouseDoubleClickEvent(QMouseEvent *event)
{
  int idx = m_transfer_func->findByPosition(toTCP(event->pos()),
                                            scaleToTCP(POINT_SEARCH_RADIUS,
                                                       POINT_SEARCH_RADIUS));
  if (idx != TransferFunction::INVALID_TCP_INDEX)
  {
    m_transfer_func->setTCPColor(idx, QColorDialog::getColor());
  }
  else
  {
    m_transfer_func->addTCP(toTCP(event->pos()));
  }

  update();
  emit transferFunctionChanged(m_transfer_func);

  return QWidget::mouseDoubleClickEvent(event);
}


void TransferFunctionEditor::mouseMoveEvent(QMouseEvent *event)
{
  if (m_cur_tcp_idx != TransferFunction::INVALID_TCP_INDEX)
  {
    m_transfer_func->setTCPPosition(m_cur_tcp_idx, toTCP(event->pos()));
    update();
    emit transferFunctionChanged(m_transfer_func);

    //qDebug() << m_transfer_func;
  }

  return QWidget::mouseMoveEvent(event);
}
