#include "logger.h"

#include <QDebug>



bool Logger::init(void)
{
  if (!initialize())
  {
    qWarning("Failed to initialize debug logger");
    return false;
  }

  connect(this, SIGNAL(messageLogged(const QOpenGLDebugMessage & )),
          this, SLOT(handleLoggedMessage(const QOpenGLDebugMessage & )),
          Qt::DirectConnection);

  startLogging(QOpenGLDebugLogger::SynchronousLogging);
  enableMessages();

  return true;
}


void Logger::handleLoggedMessage(const QOpenGLDebugMessage & msg)
{
  qDebug() << msg;
}
