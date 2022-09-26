#pragma once

#include <QWidget>
#include <QDateTime>
#define DPy_BUILD_CORE_BUILTIN
#include <Python.h>

class QTcpServer;
class QTextEdit;
class QTcpSocket;

// ======================================================================
class MyServer : public QWidget {
Q_OBJECT
private:
    QTcpServer* m_ptcpServer;
    QTextEdit*  m_ptxt;
    quint16     m_nNextBlockSize;

    QString logpass1 = "AndreyNesmeyanov&89DA234PIW9DS23";
    QString logpass2 = "Viktor&98324";
    QString logpass3 = "Alex&1234";
    QString authCode;
    QStringList users;

private:
    void sendToClient(QTcpSocket* pSocket, const QString& str);

public:
    MyServer(int nPort, QWidget* pwgt = 0);


private Q_SLOTS:
    virtual void slotNewConnection();
            void slotReadClient();
};

