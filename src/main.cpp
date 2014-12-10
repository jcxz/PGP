#include "volume_window.h"

#include <QtGui/QGuiApplication>



int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  VolumeWindow window;
  window.show();

  return app.exec();
}
