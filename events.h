#ifndef __EVENTS_H_
#define __EVENTS_H_

struct ServerEvent : public QEvent
{
  ServerEvent(const int c, const QString &msg) :
    QEvent(QEvent::Type(QEvent::User+1)),
    code(c), message(msg) {}
  
  int code;
  QString message;
};

class DefaultTransition : public QAbstractTransition
{
public:
  DefaultTransition(QLabel *l) :
    label(l) {}

protected:
  virtual bool eventTest(QEvent *e)
  {
    return (e->type() == QEvent::Type(QEvent::User+1));
  }

  virtual void onTransition(QEvent *e)
  {
    qDebug() << "on transition";
    ServerEvent *se = static_cast<ServerEvent *>(e);
    label->setText("Not implemented: " + se->message);
    qDebug() << "after";
  }

private:
  QLabel *label;
};

class CodeTransition : public QAbstractTransition
{
public:
  CodeTransition(const int c, QLabel *l) :
    code(c), label(l) {}
  
protected:
  virtual bool eventTest(QEvent *e)
  {
    if (e->type() != QEvent::Type(QEvent::User+1))
      return false;
    ServerEvent *se = static_cast<ServerEvent *>(e);
    return (code == se->code);
  }

  virtual void onTransition(QEvent *e)
  {
    ServerEvent *se = static_cast<ServerEvent *>(e);
    label->setText(se->message.trimmed());
  }

private:
  int code;
  QLabel *label;
};


    
#endif

// Local variables:
// mode: c++
// End:
