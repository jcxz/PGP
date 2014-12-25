#ifndef VOLUME_WINDOW_H
#define VOLUME_WINDOW_H

#include "opengl_window.h"
#include "texture3d.h"

#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLDebugLogger>


class VolumeWindow : public OpenGLWindow
{
    Q_OBJECT

  public:
    explicit VolumeWindow(QWindow *parent = 0);
    ~VolumeWindow(void);

    virtual void initialize(void) override;
    virtual void render(void) override;

  private slots:
    void handleLoggedMessage(const QOpenGLDebugMessage & msg);

  protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

  private:
    void genTransferFunc(void);

  private:
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_attr_pos;
    GLuint m_attr_tex_coords;
    QOpenGLShaderProgram m_program;
    Texture3D m_tex_vol_data;
    GLuint m_tex_transfer_func;
    QOpenGLDebugLogger m_logger;
    QPoint m_mouse_start;
    float m_angle_x;
    float m_angle_y;
    float m_dist_z;
    float m_depth_data;
};

#endif // VOLUME_WINDOW_H
