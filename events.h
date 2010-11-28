#ifndef __EVENTS_H_
#define __EVENTS_H_

struct ServerEvent : public QEvent
{
  ServerEvent(const int c, const QString& msg) :
    QEvent(QEvent::Type(QEvent::User+1)),
    code(c), message(msg) {}
  
  int code;
  QString message;
};

struct TextEvent : public QEvent
{
  TextEvent(const QString& t) :
    QEvent(QEvent::Type(QEvent::User+2)),
    text(t) {}

  QString text;
};

class DefaultTransition : public QAbstractTransition
{
public:
  DefaultTransition(QLabel *l) :
    label(l) {}

protected:
  virtual bool eventTest(QEvent *e)
  {
    return (e->type() == QEvent::Type(QEvent::User+1)) ||
      (e->type() == QEvent::Type(QEvent::User+2));
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

class TextTransition : public QAbstractTransition
{
public:
  TextTransition(QTextEdit *t) :
    textEdit(t) {}

protected:
  virtual bool eventTest(QEvent *e)
  {
    if (e->type() != QEvent::Type(QEvent::User+2))
      return false;
    TextEvent *te = static_cast<TextEvent *>(e);
    qDebug() << te->text << te->text.length();
    return (te->text != ".\r\n");
  }

  virtual void onTransition(QEvent *e)
  {
    TextEvent *te = static_cast<TextEvent *>(e);
    textEdit->append(te->text.trimmed());
  }

private:
  QTextEdit *textEdit;
};

class DefinitionEndTransition : public QAbstractTransition
{
public:
  DefinitionEndTransition(QTextEdit *t) :
    textEdit(t) {}

protected:
  virtual bool eventTest(QEvent *e)
  {
    if (e->type() != QEvent::Type(QEvent::User+2))
      return false;
    TextEvent *te = static_cast<TextEvent *>(e);
    return (te->text == ".\r\n");
  }

  virtual void onTransition(QEvent *)
  {
    textEdit->append("\n--------------------\n");
  }

private:
  QTextEdit *textEdit;
};
  
#endif

// Local variables:
// mode: c++
// End:
