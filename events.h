#ifndef __EVENTS_H_
#define __EVENTS_H_

const char* EOD = ".\r\n"; // Word definition ends with this

// Event with QString payload

struct ServerEvent : public QEvent
{
        ServerEvent(const QString& msg) :
                QEvent(QEvent::Type(QEvent::User+1)),
                message(msg) {}

        QString message;
};

// *** TRANSITIONS ***

// default transitions for non-implemented messages
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
                        ServerEvent *se = static_cast<ServerEvent *>(e);
                        label->setText("Not implemented: " + se->message);
                }
        private:
                QLabel *label;
};

// transition for the messages starting with given code
class CodeTransition : public QAbstractTransition
{
        public:
                CodeTransition(const QString& c, QLabel *l) :
                        code(c), label(l) {}
        protected:
                virtual bool eventTest(QEvent *e)
                {
                        if (e->type() != QEvent::Type(QEvent::User+1))
                                return false;
                        ServerEvent *se = static_cast<ServerEvent *>(e);
                        return (se->message.startsWith(code));
                }
                virtual void onTransition(QEvent *e)
                {
                        ServerEvent *se = static_cast<ServerEvent *>(e);
                        label->setText(se->message.trimmed());
                }
        private:
                QString code;
                QLabel *label;
};

// transition for message with definition text
class TextTransition : public QAbstractTransition
{
        public:
                TextTransition(QTextEdit *t) :
                        textEdit(t) {}
        protected:
                virtual bool eventTest(QEvent *e)
                {
                        if (e->type() != QEvent::Type(QEvent::User+1))
                                return false;
                        ServerEvent *se = static_cast<ServerEvent *>(e);
                        return (se->message != EOD);
                }
                virtual void onTransition(QEvent *e)
                {
                        ServerEvent *se = static_cast<ServerEvent *>(e);
                        textEdit->append(se->message.remove('\r').remove('\n'));
                }
        private:
                QTextEdit *textEdit;
};

// transition for end of definiton message
class DefinitionEndTransition : public QAbstractTransition
{
        public:
                DefinitionEndTransition(QTextEdit *t) :
                        textEdit(t) {}
        protected:
                virtual bool eventTest(QEvent *e)
                {
                        if (e->type() != QEvent::Type(QEvent::User+1))
                                return false;
                        ServerEvent *se = static_cast<ServerEvent *>(e);
                        return (se->message == EOD);
                }
                virtual void onTransition(QEvent *)
                {
                        textEdit->append("\n--------------------------------------------------------------------------------\n");
                }
        private:
                QTextEdit *textEdit;
};

#endif
