#include "volume_renderer.h"
#include "transfer_function.h"
#include "volume_data.h"
#include "ogl.h"



bool VolumeRenderer::reset(int w, int h)
{
  // query the maximum texture size
  OGLF->glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_max_texture_size);
  qDebug() << "max_texture_size=" << m_max_texture_size;

  // shader pre bounding box
  m_prog_bbox.addShaderFromSourceFile(QOpenGLShader::Vertex,   ":/src/opengl/wire_box.vert");
  m_prog_bbox.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/src/opengl/wire_box.frag");
  m_prog_bbox.link();

  m_prog_bbox.bind();
  m_prog_bbox.setUniformValue("col", QVector3D(1.0f, 0.0f, 0.0f));
  m_prog_bbox.setUniformValue("dimensions", QVector3D(1.0f, 1.0f, 1.0f));

  // set the width and the height
  m_width = w;
  m_height = h;

  return reset_impl();
}


bool VolumeRenderer::resize(QRect rect)
{
  glViewport(0, 0, rect.width(), rect.height());
  setPerspectiveProjection(rect.width(), rect.height());
  m_width = rect.width();
  m_height = rect.height();
  return resize_impl(rect);
}


void VolumeRenderer::render(const QQuaternion & rotation,
                            const QVector3D & scale,
                            const QVector3D & translation,
                            const int detail,
                            const float peel_depth)
{
  OGLF->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (!m_data.isValid())
  {
    qWarning() << "Not rendering because volume data is NOT initialized";
    return;
  }

  if (m_render_bbox) renderBBox(rotation, scale, translation);

  return render_impl(rotation, scale, translation, peel_depth, detail);
}


void VolumeRenderer::renderBBox(const QQuaternion & rotation, const QVector3D & scale, const QVector3D & translation)
{
  m_prog_bbox.bind();

  QMatrix4x4 mv;

  mv.translate(translation);
  mv.translate(0.0f, 0.0f, -1.0f);
  mv.rotate(rotation);
  mv.scale(scale);
  mv.scale(1.1f, 1.1f, 1.1f);

#if 0
  mv.scale(m_data->maxPhysicalSize() / m_data->physicalWidth(),
           m_data->maxPhysicalSize() / m_data->physicalHeight(),
           m_data->maxPhysicalSize() / m_data->physicalDepth());
#endif

  m_prog_bbox.setUniformValue("proj", m_proj);
  m_prog_bbox.setUniformValue("mv", mv);

  OGLF->glEnable(GL_DEPTH_TEST);
  OGLF->glDrawArrays(GL_LINES, 0, 24);
  OGLF->glDisable(GL_DEPTH_TEST);

  m_prog_bbox.release();
}


bool VolumeRenderer::uploadTransferFunction(const TransferFunction & transfer_func)
{
  int width = m_data.maxIntensity();

  //if (width > 16384) width = 16384;
  if (width > m_max_texture_size) width = m_max_texture_size;

  unsigned char *pixels = new unsigned char[width * 4];

  for (int i = 0; i < width; ++i)
  {
    QColor c = transfer_func.color(i, width);

    pixels[i * 4 + 0] = c.red();
    pixels[i * 4 + 1] = c.green();
    pixels[i * 4 + 2] = c.blue();
    pixels[i * 4 + 3] = (int) (transfer_func.opacity(i, width) * 255.0f);
  }

  if (!m_transfer_func.create())
  {
    delete [] pixels;
    qWarning() << "Failed to create transfer function";
    return false;
  }

  m_transfer_func.bind();

  OGLF->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  OGLF->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  OGLF->glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  OGLF->glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

  OGLF->glBindTexture(GL_TEXTURE_1D, 0);

  delete [] pixels;

  return true;
}
