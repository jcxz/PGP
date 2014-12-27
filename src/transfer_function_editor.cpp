#include "transfer_function_editor.h"

#include <QMouseEvent>
#include <QPainter>
#include <QBrush>
#include <QDebug>



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


void TransferFunctionEditor::paintEvent(QPaintEvent *event)
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

  // nakreslenie pozdia
  pen.setColor(QColor(Qt::white));
  brush.setColor(QColor(255, 255, 255, 128));
  brush.setStyle(Qt::SolidPattern);
  painter.setPen(pen);
  painter.setBrush(brush);
  painter.drawRect(rect());

  // nakreslenie krivky medzi kontrolnymi bodmi transfer funkcie
  pen.setColor(QColor(Qt::red));
  pen.setWidth(2);
  painter.setPen(pen);
  painter.setBrush(old_brush);

  const TransferFunction::tContainer & cpts = m_transfer_func.controlPoints();

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
      //QColor c(p.color());
      //c.setAlphaF(0.25f);
      QColor c(128, 128, 128, 128);
      drawPoint(painter, fromTCP(p.position()), POINT_SELECT_RADIUS, c); //QColor(Qt::black));
    }

    drawPoint(painter, fromTCP(p.position()), POINT_RENDER_RADIUS, p.color());
  }

  painter.end();
}


void TransferFunctionEditor::mousePressEvent(QMouseEvent *event)
{
  m_cur_tcp_idx = m_transfer_func.findByPosition(toTCP(event->pos()),
                                                 scaleToTCP(POINT_SEARCH_RADIUS,
                                                            POINT_SEARCH_RADIUS));

  return QWidget::mousePressEvent(event);
}


void TransferFunctionEditor::mouseMoveEvent(QMouseEvent *event)
{
  if (m_cur_tcp_idx != TransferFunction::INVALID_TCP_INDEX)
  {
    m_transfer_func.setTCPPosition(m_cur_tcp_idx, toTCP(event->pos()));
    update();
    emit transferFunctionChanged(m_transfer_func);

    //qDebug() << m_transfer_func;
  }

  return QWidget::mouseMoveEvent(event);
}


void TransferFunctionEditor::showEvent(QShowEvent *event)
{
  emit transferFunctionChanged(m_transfer_func);
  return QWidget::showEvent(event);
}
