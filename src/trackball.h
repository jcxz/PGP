/*
 * The code in this file was taken from qt boxes demo
 * and adapted for our needs. For the original code see
 * http://doc.qt.digia.com/qt/demos-boxes.html
 */

#ifndef TRACKBALL_H
#define TRACKBALL_H

#include <QVector3D>
#include <QQuaternion>


/**
 * This class takes care of scene rotations
 */
class TrackBall
{
  public:
    /**
     * Constructor
     */
    TrackBall(void)
      : m_angle(0.0f),
        m_axis(QVector3D(0, 1, 0)),
        m_last_pos(0.0f, 0.0f),
        m_rotation()
    {
    }

    /**
     * Constructor
     */
    TrackBall(float angle, const QVector3D & axis)
      : m_angle(angle),
        m_axis(axis),
        m_last_pos(0.0f, 0.0f),
        m_rotation()
    { }

    /**
     * This method will return the currently set rotation
     */
    QQuaternion getRotation(void) const
    {
      return QQuaternion::fromAxisAndAngle(m_axis, m_angle) * m_rotation;
    }

    /**
     * This method is used to record the initial position
     * which will be used for calculations in move method
     *
     * @param p the inital point which must be in range [-1, 1] x [-1, 1]
     */
    void push(QPointF p)
    {
      m_last_pos = p;
    }

    void push(QPointF p, int width, int height)
    {
      m_last_pos.setX(2.0f * float(p.x()) / float(width) - 1.0f);
      m_last_pos.setY(1.0f - 2.0f * float(p.y()) / float(height));
    }

    /**
     * coordinates have to be in [-1,1] x [-1,1] for easier math
     */
    void move(QPointF p);

    void move(QPointF p, int width, int height)
    {
      p.setX(2.0f * float(p.x()) / float(width) - 1.0f);
      p.setY(1.0f - 2.0f * float(p.y()) / float(height));
      return move(p);
    }

    /**
     * This method will reset the trackball position
     */
    void reset(void);

  private:
    float m_angle;
    QVector3D m_axis;
    QPointF m_last_pos;
    QQuaternion m_rotation;
};

#endif
