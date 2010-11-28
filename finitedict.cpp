#include <QtGui>
#include <QDebug>
#include <QtNetwork>

#include "finitedict.h"

FiniteDict::FiniteDict(QWidget *parent) : QDialog(parent)
{
        word = new QLineEdit("foo");
        statusLabel = new QLabel("disconnected");
        connectButton = new QPushButton("Connect");
        defineButton = new QPushButton("Define");
        defineButton->setDefault(true);
        defineButton->setEnabled(false);
        sendQuitButton = new QPushButton("Send quit");
        sendQuitButton->setEnabled(false);
        quitButton = new QPushButton("Quit");

        buttonBox = new QDialogButtonBox();

        buttonBox->addButton(connectButton, QDialogButtonBox::ActionRole);
        buttonBox->addButton(defineButton, QDialogButtonBox::ActionRole);
        buttonBox->addButton(sendQuitButton, QDialogButtonBox::ActionRole);
        buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

        dictSocket = new QTcpSocket(this);

        connect(connectButton, SIGNAL(clicked()), this, SLOT(doConnect()));
        connect(defineButton, SIGNAL(clicked()), this, SLOT(doDefine()));
        connect(sendQuitButton, SIGNAL(clicked()), this, SLOT(doSendQuit()));
        connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

        connect(dictSocket, SIGNAL(readyRead()), this, SLOT(processInput()));

        QGridLayout *mainLayout = new QGridLayout;
        mainLayout->addWidget(word, 0, 0);
        mainLayout->addWidget(statusLabel, 0, 1);
        mainLayout->addWidget(buttonBox, 0, 2);
        setLayout(mainLayout);

        setWindowTitle("FiniteDict");
}

void FiniteDict::doConnect()
{
        qDebug() << "Connecting...";
        textMode = false;
        disableQuit();
        statusLabel->setText("connecting...");
        dictSocket->abort();
        dictSocket->connectToHost("dict.org", 2628);
}

void FiniteDict::disableQuit()
{
        quitButton->setEnabled(false);
        connectButton->setEnabled(false);
}

void FiniteDict::enableQuit()
{
        quitButton->setEnabled(true);
        connectButton->setEnabled(true);
        defineButton->setEnabled(false);
        sendQuitButton->setEnabled(true);
}

void FiniteDict::processInput()
{
        qDebug() << "got something";
        if (!dictSocket->canReadLine())
                return;

        char buf[1024];

        dictSocket->readLine(buf, sizeof(buf));

        QString answer = QString::fromUtf8(buf);

#define CODE(X) else if (answer.startsWith(#X))

        if (textMode)
                if (answer == ".\n")
                        ;//send . message
                else {
                        ;// send text message
                }
        CODE(220) {
                ;// connected
        }
        CODE(550) {
                ;
        }
        CODE(551) {
                ;
        }
        CODE(552) {
                ;
        }
        CODE(150) {
                ;
        }
        CODE(151) {
                ;
        }
        CODE(250) {
                ;
        }
        CODE(221) {
                ;
        }
        else {
                ; // unknown response
        }
}

void FiniteDict::doDefine()
{
}

void FiniteDict::doSendQuit()
{
}
