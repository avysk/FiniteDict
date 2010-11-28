#include <QtGui>
#include <QtNetwork>

#include "finitedict.h"
#include "events.h"

FiniteDict::FiniteDict(QWidget *parent) : QDialog(parent)
{
  // Create GUI
  word = new QLineEdit("foo");
  statusLabel = new QLabel("disconnected");
  
  connectButton = new QPushButton("Connect");
  defineButton = new QPushButton("Define");
  sendQuitButton = new QPushButton("Send quit");
  quitButton = new QPushButton("Quit");

  connect(connectButton, SIGNAL(clicked()), this, SLOT(doConnect()));
  connect(defineButton, SIGNAL(clicked()), this, SLOT(doDefine()));
  connect(sendQuitButton, SIGNAL(clicked()), this, SLOT(doSendQuit()));
  connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

  buttonBox = new QDialogButtonBox();
  
  buttonBox->addButton(connectButton, QDialogButtonBox::ActionRole);
  buttonBox->addButton(defineButton, QDialogButtonBox::ActionRole);
  buttonBox->addButton(sendQuitButton, QDialogButtonBox::ActionRole);
  buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

  outputText = new QTextEdit();
  outputText->setLineWrapMode(QTextEdit::NoWrap);
  outputText->setReadOnly(true);  
  
  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addWidget(word, 0, 0);
  mainLayout->addWidget(statusLabel, 1, 0);
  mainLayout->addWidget(outputText, 2, 0);
  mainLayout->addWidget(buttonBox, 3, 0);
  setLayout(mainLayout);
  
  setWindowTitle("FiniteDict");
  
  // Socket for connection with dict server
  dictSocket = new QTcpSocket(this);
  
  connect(dictSocket, SIGNAL(readyRead()), this, SLOT(processInput()));

  // State machine
  initStateMachine();
  machine.start();
}

// *** STATE MACHINE ***

void FiniteDict::initStateMachine()
{
  // states
  startState = new QState();

  groupState = new QState();
  
  readyState = new QState(groupState);
  waitingState = new QState(groupState);
  betweenDefinitionsState = new QState(groupState);
  inDefinitionState = new QState(groupState);
  quittingState = new QState(groupState);

  groupState->setInitialState(readyState);

  // effects of states on GUI
  QObject::connect(startState, SIGNAL(entered()),
		   this, SLOT(resetSocket()));
  QObject::connect(startState, SIGNAL(entered()),
		   this, SLOT(guiDisconnected()));
  QObject::connect(readyState, SIGNAL(entered()),
		   this, SLOT(guiReady()));
  QObject::connect(waitingState, SIGNAL(entered()),
		   this, SLOT(guiWaiting()));
  QObject::connect(quittingState, SIGNAL(entered()),
		   this, SLOT(guiWaiting()));
  // _ clear the text field before printing next definition
  QObject::connect(waitingState, SIGNAL(entered()),
		   outputText, SLOT(clear()));

  // user-initiated transitions
  readyState->addTransition(defineButton, SIGNAL(clicked()),
			    waitingState);
  readyState->addTransition(sendQuitButton, SIGNAL(clicked()),
			    quittingState);

  // server-initiated transitions

  CodeTransition *tmp;

// HELPFUL MACROS
// Creates transition 'from' -> 'to' for the server message
// with given 'code' in the beginning of the line
#define TR(code, from, to)			\
  tmp = new CodeTransition(code, statusLabel);	\
  tmp->setTargetState(to);			\
  from->addTransition(tmp)
//
// MACROS ENDS HERE
//

  // _ messages with number code 
  TR("150", waitingState, betweenDefinitionsState);
  TR("151", betweenDefinitionsState, inDefinitionState); 
  TR("220", startState, readyState);
  TR("221", quittingState, startState);
  TR("250", betweenDefinitionsState, readyState);
  TR("552", waitingState, readyState);
  
  // _ definition text transition
  TextTransition *tr_text = new TextTransition(outputText);
  tr_text->setTargetState(inDefinitionState);
  inDefinitionState->addTransition(tr_text);

  // _ end of definiton transition
  DefinitionEndTransition *tr_end =
    new DefinitionEndTransition(outputText);
  tr_end->setTargetState(betweenDefinitionsState);
  inDefinitionState->addTransition(tr_end);

  // _ unknown messages
  DefaultTransition *tr_nip = new DefaultTransition(statusLabel);
  tr_nip->setTargetState(startState);
  groupState->addTransition(tr_nip);

  // Putting it all togehther
  machine.addState(groupState);
  machine.addState(startState);
  machine.setInitialState(startState);
}


// *** SLOTS ***

// buttons actions

void FiniteDict::doConnect()
{
  statusLabel->setText("connecting...");
  dictSocket->connectToHost("dict.org", 2628);
}

void FiniteDict::doDefine()
{
  dictSocket->write("DEFINE ! " + word->text().toUtf8() + "\n");
}

void FiniteDict::doSendQuit()
{
  const char *cmdQuit = "QUIT\n";
  dictSocket->write(cmdQuit);
}

// socket

void FiniteDict::processInput()
{
  char buf[1024]; // FOR SIMPLICITY, SHOULD NOT BE DONE LIKE THIS!

  while (dictSocket->canReadLine()) {
    dictSocket->readLine(buf, sizeof(buf));
    QString answer = QString::fromUtf8(buf);
    machine.postEvent(new ServerEvent(answer));
  }
}

void FiniteDict::resetSocket()
{
  dictSocket->abort();
}

// GUI

// _ disconnected
void FiniteDict::guiDisconnected()
{
  connectButton->setEnabled(true);
  connectButton->setDefault(true);
  defineButton->setEnabled(false);
  sendQuitButton->setEnabled(false);
  quitButton->setEnabled(true);
  word->setEnabled(false);
}

// _ connected, ready to search
void FiniteDict::guiReady()
 {
  connectButton->setEnabled(false);
  defineButton->setEnabled(true);
  defineButton->setDefault(true);
  sendQuitButton->setEnabled(true);
  quitButton->setEnabled(false);
  word->setEnabled(true);
  word->setFocus();
  word->selectAll();
}

// _ blocked, waiting for server
void FiniteDict::guiWaiting()
 {
  connectButton->setEnabled(false);
  defineButton->setEnabled(false);
  sendQuitButton->setEnabled(false);
  quitButton->setEnabled(false);
  word->setEnabled(false);
}


