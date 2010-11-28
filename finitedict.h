#ifndef __FINITEDICT_H_
#define __FINITEDICT_H_

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QString>
#include <QTcpSocket>
#include <QStateMachine>
#include <QState>

class FiniteDict : public QDialog
{
        Q_OBJECT

        public:
                FiniteDict(QWidget *parent = 0);

        private slots:
                void doConnect();
                void doDefine();
                void doSendQuit();
                void disableQuit();
                void enableQuit();
                void processInput();

        private:
                QLineEdit *word;
                QPushButton *connectButton;
                QPushButton *defineButton;
                QPushButton *sendQuitButton;
                QPushButton *quitButton;
                QLabel *statusLabel;
                QDialogButtonBox *buttonBox;

                QTcpSocket *dictSocket;

                QStateMachine machine;

                QState *startState;
                QState *readyState;
                QState *waitingState;
                QState *betweenDefinitionsState;
                QState *inDefinitionState;
                QState *quittingState;
                QState *finalState;

                bool textMode;
};

#endif
