#pragma once

#include <QWidget>
#include <QTcpSocket>
#include <QDebug>
#include <QMessageBox>
#include <QMessageBox>

class QTextEdit;
class QLineEdit;

// ======================================================================
class MyClient : public QWidget {
Q_OBJECT
private:
    QTcpSocket* m_pTcpSocket;
    QTextEdit*  m_ptxtInfo;
    QLineEdit*  m_ptxtInputLogin;
    QLineEdit*  m_ptxtInputPassword;
    QLineEdit*  m_ptxtInputCode;
    quint16     m_nNextBlockSize;

    QPushButton* pcmd1;

public:
    MyClient(const QString& strHost, int nPort, QWidget* pwgt = 0) ;

private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void slotSendToServer();
    void slotSendToServer1();
    void slotConnected();
};
