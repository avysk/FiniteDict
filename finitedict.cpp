#include <QtGui>
#include <QDebug>
#include <QtNetwork>

#include "finitedict.h"
#include "events.h"

FiniteDict::FiniteDict(QWidget *parent) : QDialog(parent)
{

  initStateMachine();

  word = new QLineEdit("foo");
  statusLabel = new QLabel("disconnected");
  connectButton = new QPushButton("Connect");
  defineButton = new QPushButton("Define");
  sendQuitButton = new QPushButton("Send quit");
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

  machine.start();
}

void FiniteDict::resetClient()
{
  qDebug() << "Client reset.";
  textMode = false;
  dictSocket->abort();
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

#define CODE(X) else if (answer.startsWith(X))	\
    machine.postEvent(new ServerEvent<X>(answer))

  if (textMode)
    if (answer == ".\n")
      ;//send . message
    else {
      ;// send text message
    }
    CODE(150);
    CODE(151);
    CODE(220);
    CODE(221);
    CODE(250);
    CODE(550);
    CODE(551);
    CODE(552);
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

void FiniteDict::guiDisonnected()
{
  connectButton->setEnabled(true);
  defineButton->setEnabled(false);
  sendQuitButton->setEnabled(false);
  quitButton->setEnabled(true);
  word->setEnabled(false);
}

void FiniteDict::guiQuitting()
{
  connectButton->setEnabled(false);
  defineButton->setEnabled(false);
  sendQuitButton->setEnabled(false);
  quitButton->setEnabled(false);
  word->setEnabled(false);
}

void FiniteDict::guiReady()
{
  connectButton->setEnabled(false);
  defineButton->setEnabled(true);
  sendQuitButton->setEnabled(true);
  quitButton->setEnabled(false);
  word->setEnabled(true);
}

void FiniteDict::guiWaiting()
{
  connectButton->setEnabled(false);
  defineButton->setEnabled(false);
  sendQuitButton->setEnabled(false);
  quitButton->setEnabled(false);
  word->setEnabled(false);
}

void FiniteDict::initStateMachine()
{
  startState = new QState();

  groupState = new QState();
  readyState = new QState(groupState);
  waitingState = new QState(groupState);
  betweenDefinitionsState = new QState(groupState);
  inDefinitionState = new QState(groupState);
  quittingState = new QState(groupState);

  QObject::connect(startState, SIGNAL(entered()), this, SLOT(resetClient()));
  QObject::connect(startState, SIGNAL(entered()), this, SLOT(guiDisconnected()));
  QObject::connect(readyState, SIGNAL(entered()), this, SLOT(guiReady()));
  QObject::connect(waitingState, SIGNAL(entered()), this, SLOT(guiWaiting()));
  QObject::connect(quittingState, SIGNAL(entered()), this, SLOT(guiQuitting()));

  readyState->addTransition(defineButton, SIGNAL(clicked()), waitingState);
  readyState->addTransition(sendQuitButton, SIGNAL(clicked()), quittingState);



  groupState->setInitialState(startState);

  machine.addState(groupState);
  machine.addState(startState);
  machine.setInitialState(startState);

}
