#ifndef LOGGER_H
#define LOGGER_H

#include <QOpenGLDebugLogger>


class Logger : public QOpenGLDebugLogger
{
    Q_OBJECT

  public:
    explicit Logger(QObject *parent = 0)
      : QOpenGLDebugLogger(parent)
    { }

    bool init(void);

  private slots:
    void handleLoggedMessage(const QOpenGLDebugMessage & msg);
};

#endif // LOGGER_H
