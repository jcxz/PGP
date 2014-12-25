/*
 * The code in this file was taken from qt boxes demo
 * and adapted for our needs. For the original code see
 * http://doc.qt.digia.com/qt/demos-boxes.html
 */

#include "trackball.h"

#include <cmath>

#include <QLineF>


static const double PI = 3.14;



void TrackBall::reset(void)
{
  m_rotation = QQuaternion();
  m_axis = QVector3D(0, 1, 0);
  m_angle = 0.0f;
  m_last_pos = QPointF(0.0f, 0.0f);
}


void TrackBall::move(QPointF p)
{
  QVector3D last_pos_3D = QVector3D(m_last_pos.x(), m_last_pos.y(), 0.0f);
  float sqr_z = 1 - QVector3D::dotProduct(last_pos_3D, last_pos_3D);
  if (sqr_z > 0)
  {
    last_pos_3D.setZ(sqrt(sqr_z));
  }
  else
  {
    last_pos_3D.normalize();
  }

  QVector3D current_pos_3D = QVector3D(p.x(), p.y(), 0.0f);
  sqr_z = 1 - QVector3D::dotProduct(current_pos_3D, current_pos_3D);
  if (sqr_z > 0)
  {
    current_pos_3D.setZ(sqrt(sqr_z));
  }
  else
  {
    current_pos_3D.normalize();
  }

  m_axis = QVector3D::crossProduct(last_pos_3D, current_pos_3D);
  m_angle = 180 / PI * asin(sqrt(QVector3D::dotProduct(m_axis, m_axis)));
  m_axis.normalize();
  m_axis = QQuaternion().rotatedVector(m_axis);
  m_rotation = QQuaternion::fromAxisAndAngle(m_axis, m_angle) * m_rotation;

  m_last_pos = p;
}
