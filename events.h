#ifndef __EVENTS_H_
#define __EVENTS_H_

int code_to_num(int code)
{
  switch (code) {
  case 150: return 1;
  case 151: return 2;
  case 220: return 3;
  case 221: return 4;
  case 250: return 5;
  case 550: return 6;
  case 551: return 7;
  case 552: return 8;
  default: return 9;
  }
}

template<int i> struct ServerEvent : public QEvent
{
  ServerEvent(const QString &msg) :
    QEvent(QEvent::Type(QEvent::User+code_to_num(i))),
    message(msg) {}
  
  QString message;
};

template<int i> class myTransition : public QAbstractTransition
{
public:
  myTransition() {}
  
protected:
  virtual bool eventTest(QEvent *e) const
  {
    return (e->type() == QEvent::Type(QEvent::User+code_to_num(i)));
  }
};
    
#endif

// Local variables:
// mode: c++
// End:
