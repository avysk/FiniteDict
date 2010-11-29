#ifndef __FINITEDICT_H_
#define __FINITEDICT_H_

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QSignalMapper>
#include <QState>
#include <QStateMachine>
#include <QString>
#include <QTcpSocket>
#include <QTextEdit>

class FiniteDict : public QDialog
{
        Q_OBJECT

        public:

                FiniteDict(QWidget *parent = 0);

        private slots:

                // Buttons actions
                void doConnect();
                void doDefine();
                void doSendCmd(QString);

                // Socket
                void processInput();
                void resetSocket();

                // GUI
                void guiConnected();
                void guiDisconnected();
                void guiReady();
                void guiWaiting();

        private:

                // GUI elements
                QLineEdit *word;
                QPushButton *connectButton;
                QPushButton *sendClientButton;
                QPushButton *defineButton;
                QPushButton *sendQuitButton;
                QPushButton *quitButton;
                QLabel *statusLabel;
                QTextEdit *outputText;
                QDialogButtonBox *buttonBox;

                // socket
                QTcpSocket *dictSocket;

                // State machine
                QStateMachine machine;
                // _ states
                QState *startState;
                QState *connectedState;
                QState *readyState;
                QState *waitingState;
                QState *betweenDefinitionsState;
                QState *inDefinitionState;
                QState *quittingState;
                QState *groupState;

                void initStateMachine();

                QSignalMapper *cmdMapper;

};

#endif
