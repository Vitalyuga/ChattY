/*
 FILNAMN: 		server.cc
 PROGRAMMERARE:	hanel742, eriek984, jened502, tobgr602, niker917, davha227
 SKAPAD DATUM:	2013-11-18
 BESKRIVNING:
 */

#include "thread.h"

using namespace std;


// ---------------------------------------
// Helper functions

void Thread::handleMessage(QString inData){
    int i;
    
    QString from;
    i = inData.indexOf(compare);
    from = inData.left(i);
    string stdFrom = from.toStdString();
    inData = inData.mid(i+1);
    
    QString to;
    i = inData.indexOf(compare);
    to = inData.left(i);
    string stdTo = to.toStdString();
    inData = inData.mid(i+1);
    
    QString contents;
    i = inData.indexOf(compare);
    contents = inData.left(i);
    string stdContents = contents.toStdString();
    inData = inData.mid(i+1);
    
    Message message(stdContents, stdFrom, stdTo);
    userPointer->sendMessage(message);
}

// ----------------------

void Thread::handleInitiate(string stdInData) {
    try
    {
        userPointer = masterPointer->createUser(stdInData);
        userPointer->setThread(this);
        handleStructure();
        userPointer->sendHistory();
    }
    catch (...)
    {
        reinitiate();
    }
}

// ----------------------

void Thread::handleStructure() {
    cout << "Thread::handleStructure" << endl;
    vector<string> structure = userPointer->getStruct();

    QByteArray sendData;
    sendData += "/structure";
    sendData += compare;
    unsigned int i;
    
    for (i = 0; i < (structure.size()-1)  ; i++) {
        sendData += QString::fromStdString(structure.at(i));
        sendData += compare;
    }
    sendData += QString::fromStdString(structure.at(i));
    sendData += breaker;
    
    cout << "User: " << structure.at(2  ) << endl;
    TcpSocket->write(sendData);
    
    cout << "Struct sent" << endl;
    
}


// ---------------------------------------

Thread::Thread(qintptr ID, Master* masterptr, QObject *parent) : QThread(parent)
{
    masterPointer=masterptr;
    this->socketDescriptor = ID;
    compare += 0x1F;
    breaker += 0x1E;
}

// ---------------------------------------

void Thread::run()
{
    //thread starts here
    TcpSocket = new QTcpSocket();
    
    
    if(!TcpSocket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(TcpSocket->error());
        return;
    }
    
    connect(TcpSocket,SIGNAL(readyRead()),this,SLOT(readyRead()),Qt::DirectConnection);
    
    connect(TcpSocket,SIGNAL(disconnected()),this,SLOT(disconnected()),Qt::DirectConnection);
    
    //creates a messageloop
    exec();
    
}

// ---------------------------------------


// ---------------------------

void Thread::readyRead()
{
    
    QByteArray Data = TcpSocket->readAll();
    
    int i;
    int n;
    
    QString commandName;
    QString inData = Data;
    
    QString rest;
    
    while ( !inData.isEmpty() ) {
        
        i = inData.indexOf(compare);
        
        commandName = inData.left(i);
        inData = inData.mid(i+1);
        
        n = inData.indexOf(breaker);
        
        if (inData.size() < 2) {
            
            break;
        }
        rest = inData.mid(n+1);
        
        inData = inData.left(n);
        
        QString temp = inData;
        string stdInData = temp.toStdString();
        
        // Check which command that's supposed to run
        if (commandName == "/initiate") {
            handleInitiate(stdInData);
        }
        
        else if (commandName == "/message") {
            qDebug() << "/message thread";
            handleMessage(inData);
        }
        
        else if ( commandName == "/structure" ) {
            handleStructure();
            cout << "Handled structure, but not really" << endl;
        }
        
        else {
            cout << "Ej giltigt kommando" << commandName.toStdString() << endl;
            TcpSocket->write("Ej giltigt kommando");
            cout << socketDescriptor << "Data in: "<< stdInData<<endl;
        }
        
        inData = rest;
        
    }
}

// ---------------------------------------

void Thread::disconnected()
{
    cout << socketDescriptor << "Disconnected"<<endl;
    try {
        masterPointer->removeUser(userPointer->getName());
    } catch (...) {
        
    }
    
    TcpSocket->deleteLater();
    //exits the thread
    exit(0);
}

// -----------------------------------------

//svarar klienten
void Thread::sendMessage(Message messageObject){
    QByteArray array = "/message";
    array += 0x1F; //unit separator
    array += QString::fromStdString(messageObject.getFrom());
    array += 0x1F;
    array += QString::fromStdString(messageObject.getTo());
    array += 0x1F;
    array += QString::fromStdString(messageObject.getMessage());
    array += 0x1F;
    array += QString::fromStdString(messageObject.getServerTime());
    array += 0x1E;
    
    TcpSocket->write(array);
    TcpSocket->waitForBytesWritten(1000);
    
}

// -----------------------------------------

void Thread::sendHistory(){
    QByteArray array = "/history";
    array += 0x1F; //unit separator
    unsigned int logSize = userPointer->getParentRoom()->log.size();
    
    for (unsigned int i = 0; i < logSize; i++){
        
        Message tempMessage = userPointer->getParentRoom()->log.at(i);
        
        array += QString::fromStdString(tempMessage.getFrom());
        array += 0x1F; //unit separator
        array += QString::fromStdString(tempMessage.getTo());
        array += 0x1F; //unit separator
        array += QString::fromStdString(tempMessage.getMessage());
        array += 0x1F; //unit separator
        array += QString::fromStdString(tempMessage.getServerTime());
        if ( i+1 == logSize ){
            array += 0x1E;
            break;
        }
        array += 0x1F; //unit separator
    }
    TcpSocket->write(array);
    TcpSocket->waitForBytesWritten(1000);
    
}

// -----------------------------------------

void Thread::reinitiate(){
    QByteArray array = "/reinitiate";
    array += 0x1F;
    array += 0x1E; //unit separator
    
    TcpSocket->write(array);
    TcpSocket->waitForBytesWritten(1000);
}

// -----------------------------------------


void Thread::requestStruct() {
    QByteArray array = "/structure";
    array += 0x1F;
    array += 0x1E;
    
    TcpSocket->write(array);
    TcpSocket->waitForBytesWritten(1000);
}
