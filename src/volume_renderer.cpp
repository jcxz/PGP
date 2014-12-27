#include "volume_renderer.h"
#include "transfer_function.h"



bool VolumeRenderer::uploadTransferFunction(const TransferFunction & transfer_func)
{
  const int width = 256;

  unsigned char *pixels = new unsigned char[width * 4];

  for (int i = 0; i < width; ++i)
  {
    QColor c = transfer_func.color(i);

    pixels[i * 4 + 0] = c.red();
    pixels[i * 4 + 1] = c.green();
    pixels[i * 4 + 2] = c.blue();
    pixels[i * 4 + 3] = (int) (transfer_func.opacity(i) * 255.0f);
  }

  if (!m_transfer_func.create())
  {
    delete [] pixels;
    qWarning() << "Failed to create transfer function";
    return false;
  }

  m_transfer_func.bind();

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

  glBindTexture(GL_TEXTURE_1D, 0);

  delete [] pixels;

  return true;
}
