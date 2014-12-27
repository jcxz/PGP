#include "transfer_function_editor.h"

#include <QMouseEvent>
#include <QPainter>
#include <QBrush>
#include <QDebug>


namespace {

constexpr int POINT_RENDER_RADIUS = 2;
constexpr int POINT_SEARCH_RADIUS = 4;

} // End of private namespace


/*
TransferFunctionEditor::TransferPoint *TransferFunctionEditor::findPointAt(QPoint mouse_pos)
{
  qDebug() << "Mouse pos: " << mouse_pos;

  for (TransferPoint & pt : m_transfer_points)
  {
    QPoint pos = pt.position();
    if ((abs(pos.x() - mouse_pos.x()) <= POINT_SEARCH_RADIUS) &&
        (abs(pos.y() - mouse_pos.y()) <= POINT_SEARCH_RADIUS))
    {
      qDebug() << "pos=" << pos;
      return &pt;
    }
  }

  return nullptr;
}
*/


void TransferFunctionEditor::paintEvent(QPaintEvent *event)
{
  //if (m_transfer_points.isEmpty())
  //{
  //  qDebug() << "Not drawing: No transfer control points were set";
  //  return;
  //}

  QPainter painter;

  painter.begin(this);

  // zapnutie antialiasingu
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::HighQualityAntialiasing);

  //QBrush old_brush = painter.brush();
  //QPen old_pen = painter.pen();

  // nakreslenie krivky medzi kontrolnymi bodmi transfer funkcie
  QPen pen(QColor(Qt::red));
  pen.setWidth(2);

  painter.setPen(pen);

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

    QPen pen(p.color());
    pen.setWidth(5);

    QBrush brush(p.color());

    painter.setPen(pen);
    painter.setBrush(brush);
    //painter.drawPoint(p.position());
    painter.drawEllipse(fromTCP(p.position()), POINT_RENDER_RADIUS, POINT_RENDER_RADIUS);
  }

  //painter.setPen(old_pen);
  //painter.setBrush(old_brush);

  painter.end();
}


void TransferFunctionEditor::mousePressEvent(QMouseEvent *event)
{
  m_cur_pt = m_transfer_func.findByPosition(toTCP(event->pos()),
                                            toTCP(QPoint(POINT_SEARCH_RADIUS,
                                                         POINT_SEARCH_RADIUS)));
  return QWidget::mousePressEvent(event);
}


void TransferFunctionEditor::mouseMoveEvent(QMouseEvent *event)
{
  if (m_cur_pt != nullptr)
  {
    m_cur_pt->setPosition(toTCP(event->pos()));
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
